package net.rpdev.OpenTodoList;

import android.content.Context;
import android.security.KeyPairGeneratorSpec;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.KeyPairGenerator;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PublicKey;
import java.security.UnrecoverableEntryException;
import java.security.cert.CertificateException;
import java.util.Calendar;

import javax.crypto.Cipher;
import javax.crypto.CipherInputStream;
import javax.crypto.CipherOutputStream;
import javax.crypto.NoSuchPaddingException;
import javax.security.auth.x500.X500Principal;

/**
 * @brief Utilities to encrypt and decrypt information.
 *
 * This helper class is used by the C++ side of the app to encrypt sensitive information and store
 * it securely for later.
 */
class CryptoUtils {

    /**
     * @brief Holds the result of an encryption procedure.
     */
    public static class EncryptionResult {
        public byte[] cipherText;
        boolean success;

        public EncryptionResult() {
            cipherText = new byte[0];
            success = false;
        }
    }

    /**
     * @brief Holds the result of a decryption procedure.
     */
    public static class DecryptionResult {
        public byte[] plainText;
        boolean success;

        public DecryptionResult() {
            plainText = new byte[0];
            success = false;
        }
    }

    /**
     * @brief Encrypt data.
     *
     * This method encrypts the @p plainText, creating - if necessary - a new key pair bound to the
     * @p service name and @p key, which are meta data used to identify the data to be encrypted.
     *
     * If the encryption fails, this method first tries to remove potentially existing stale old
     * keys for the data and retries the encryption.
     */
    public static EncryptionResult encrypt(
            Context context,
            String service,
            String key,
            byte[] plainText) {
        EncryptionResult result = encryptInner(context, service, key, plainText);
        if (!result.success) {
            // Could be old key, changed parameters, ...
            // Try to remove the key and retry:
            if (deleteKey(service, key)) {
                return encryptInner(context, service, key, plainText);
            }
        }
        return result;
    }

    /**
     * @brief Decrypt data.
     *
     * This is the reverse of the encrypt method: It takes a @p cipherText and decrypts it by
     * looking up a key pair using the @p service name and @p key (which are meta data identifying
     * the data to be protected).
     */
    public static DecryptionResult decrypt(String service, String key, byte[] cipherText) {
        DecryptionResult result = new DecryptionResult();
        try {
            KeyStore keyStore = KeyStore.getInstance("AndroidKeyStore");
            keyStore.load(null);
            String alias = makeAlias(service, key);
            KeyStore.Entry entry = keyStore.getEntry(alias, null);
            if (entry instanceof KeyStore.PrivateKeyEntry) {
                Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
                cipher.init(Cipher.DECRYPT_MODE, ((KeyStore.PrivateKeyEntry)entry).getPrivateKey());
                CipherInputStream inputStream = new CipherInputStream(
                        new ByteArrayInputStream(cipherText), cipher);
                ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                while (true) {
                    int nextByte = inputStream.read();
                    if (nextByte == -1) {
                        break;
                    } else {
                        outputStream.write(nextByte);
                    }
                }
                result.plainText = outputStream.toByteArray();
                result.success = true;
            }
        } catch (UnrecoverableEntryException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (CertificateException e) {
            e.printStackTrace();
        } catch (KeyStoreException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return result;
    }

    /**
     * @brief Delete a key pair.
     *
     * This method deletes the key pair which is used to protect the data identified by the
     * @p service name and @p key meta data.
     */
    public static boolean deleteKey(
            String service,
            String key) {
        try {
            KeyStore keyStore = KeyStore.getInstance("AndroidKeyStore");
            String alias = makeAlias(service, key);
            if (keyStore.isKeyEntry(alias)) {
                keyStore.deleteEntry(alias);
            }
            return true;
        } catch (KeyStoreException e) {
            e.printStackTrace();
        }
        return false;
    }

    private static String makeAlias(String service, String key) {
        return service + "/" + key;
    }

    private static void createKeyPair(Context context, String alias)
            throws InvalidAlgorithmParameterException,
            NoSuchProviderException,
            NoSuchAlgorithmException {
        Calendar start = Calendar.getInstance();
        Calendar end = Calendar.getInstance();
        end.add(Calendar.YEAR, 99);

        KeyPairGeneratorSpec.Builder builder = new KeyPairGeneratorSpec.Builder(context);
        builder.setAlias(alias);
        builder.setSubject(new X500Principal("CN=OpenTodoList, O=Android Authority"));
        builder.setSerialNumber(BigInteger.ONE);
        builder.setStartDate(start.getTime());
        builder.setEndDate(end.getTime());
        KeyPairGeneratorSpec spec = builder.build();

        KeyPairGenerator generator = KeyPairGenerator.getInstance(
                "RSA",
                "AndroidKeyStore");
        generator.initialize(spec);

        generator.generateKeyPair();
    }

    private static EncryptionResult encryptInner(
            Context context,
            String service,
            String key,
            byte[] plainText) {
        EncryptionResult result = new EncryptionResult();
        try {
            KeyStore keystore = KeyStore.getInstance("AndroidKeyStore");
            keystore.load(null);
            String alias = makeAlias(service, key);
            if (!keystore.containsAlias(alias)) {
                createKeyPair(context, alias);
            }
            KeyStore.Entry entry = keystore.getEntry(alias, null);
            if (entry instanceof  KeyStore.PrivateKeyEntry) {
                PublicKey publicKey = ((KeyStore.PrivateKeyEntry)entry)
                        .getCertificate().getPublicKey();
                Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
                cipher.init(Cipher.ENCRYPT_MODE, publicKey);
                ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                CipherOutputStream cipherOutputStream = new CipherOutputStream(outputStream, cipher);
                cipherOutputStream.write(plainText);
                cipherOutputStream.close();
                result.cipherText = outputStream.toByteArray();
                result.success = true;
            }
        } catch (KeyStoreException | CertificateException | IOException |
                NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        } catch (UnrecoverableEntryException e) {
            e.printStackTrace();
        } catch (InvalidAlgorithmParameterException e) {
            e.printStackTrace();
        } catch (NoSuchPaddingException e) {
            e.printStackTrace();
        } catch (NoSuchProviderException e) {
            e.printStackTrace();
        }
        return result;
    }
}
