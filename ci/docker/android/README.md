The Dockerfile and script for extraction are based on the ones found here:

https://github.com/rabits/dockerfiles/tree/master/5.14-android

To build the image, put a file `secrets.env` next to the Dockerfile with the following content

```
export QT_CI_LOGIN='YOUR_EMAIL'
export QT_CI_PASSWORD='YOUR_PASSWORD'
```

and run

```bash
make build
```

