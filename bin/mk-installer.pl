#!/usr/bin/perl

=head1 SYNOPSIS

mk-installer.pl OPTIONS

=head1 DESCRIPTION

Creates the OpenTodoList installer.

=cut

use strict;
use warnings;
use utf8;

use Config;
use Cwd qw( abs_path );
use File::Basename;
use File::Copy qw( copy );
use File::Spec;
use File::Temp qw( tempdir );
use Getopt::Long;
use Pod::Usage;
use POSIX qw( localtime strftime );

=head1 OPTIONS

=over 4

=item --arch ARCH

Specifies the architecture for which the installer is build. Should be something
like "i686" or "x86_64".

=cut

my $arch;

=item --copy-qt-menu-nib

MacOS Fix: Copy qt_menu.nib from Qt Installer Framework
installation directory. This tries to find the QtIFW by searching
in your PATH.

=cut

my $copyQtMenuNib;

=item --dmg

MacOS: Create Disk Images (*.dmg).

=cut

my $dmg;

=item --help

Shows the build in help.

=cut

my $help;

=item --installDir DIR

The directory where the OpenTodoList binaries have been installed to using
B<make install INSTALL_ROOT=/target/dir/for/install>.

=cut

my $installDir;

=item --man

Shows the scripts manual page.

=cut

my $man;

=item --offline

Create the offline installer as offline-only.
(Use this for debugging.)

=cut

my $offlineOnly;

=item --os OS

Specifies the operating system to build the repository for. Will be used in some file names.
Could be something like "macos", "windows", "linux"...

=cut

my $os;

=item --sourceDir DIR

Use config templates from DIR. If omitted, files are taken automatically relative
to the script path.

=cut

my $sourceDir = File::Spec->catfile( dirname(abs_path($0)), "..",  "templates", "installer" );

=item --targetDir DIR

Create the installer in DIR.

=cut

my $targetDir;

=item --tempDir DIR

Create temporary files in DIR. If this is not specified, a temporary directory will
be created automatically. This directory will also be removed after the script finished.
If you specify your own temporary directory, it will B<not> be removed after the script
finishes. (Use this for debugging.)

=cut

my $tempDir;

=item --update

Create an update repository (instead of recreating it).
Currently not working!

=cut

my $update;

=item --version VERSION

Use VERSION as version (instead of getting it from the built binary).

=cut

my $version;

=back

=cut

sub createTargetConfig($$$) {
    my ( $infile, $outfile, $args ) = @_;
    open( INFILE, "<", $infile ) || die( $! );
    open( OUTFILE, ">", $outfile ) || die( $! );
    while ( <INFILE> ) {
        for my $key ( keys( %{ $args } ) ) {
            my $value = $args->{ $key };
            $_ =~ s/$key/$value/g;
        }
        print OUTFILE $_;
    }
    close( OUTFILE );
    close( INFILE );
}

my $argParseError;

GetOptions(
            "arch=s" => \$arch,
            "copy-qt-menu-nib" => \$copyQtMenuNib,
            "dmg" => \$dmg,
            "help" => \$help,
            "installDir=s" => \$installDir,
            "man" => \$man,
            "offline-only" => \$offlineOnly,
            "os=s" => \$os,
            "sourceDir=s" => \$sourceDir,
            "targetDir=s" => \$targetDir,
            "tempDir=s" => \$tempDir,
            "update" => \$update,
            "version=s" => \$version,
            )
|| die( "Invalid arguments passed!" );

pod2usage( -exitval => 0, -verbose => 1 ) if $help;
pod2usage( -exitVal => 0, -verbose => 2 ) if $man;

$argParseError = "No arch specified!" if !defined( $arch );
$argParseError = "No installDir specified!" if !defined( $installDir );
$argParseError = "No os specified!" if !defined( $os );
$argParseError = "No targetDir specified!" if !defined( $targetDir );

$argParseError = "The installDir does not exist!" if ! -d $installDir;
$argParseError = "The targetDir does not exist!" if ! -d $targetDir;

if ( defined($argParseError) ) {
    pod2usage( -exitval => 1,
               -verbose => 1,
               -msg => $argParseError );
}

if ( !$tempDir ) {
    $tempDir = tempdir( CLEANUP => 1 );
}

# Get OpenTodoList version
my $openTodoListVersion = $version;
if (!defined($openTodoListVersion)) {
  printf( "Getting current OpenTodoList version...\n" );
  my $openTodoListApp = File::Spec->catfile( $installDir, "OpenTodoList", "bin", "OpenTodoList" );
  if ( ! -f $openTodoListApp ) {
      # Could we be on a Mac? Try app bundle:
      $openTodoListApp = File::Spec->catfile( $installDir,
        "Contents", "MacOS", "OpenTodoList" );
  }
  $openTodoListVersion = ( -f $openTodoListApp ? `$openTodoListApp --version` : undef );
  if ( $openTodoListVersion =~ m/^OpenTodoList\s*version\s*(.*)$/ ) {
      $openTodoListVersion = $1;
  } else {
      # Okay, we are in Windows... try to read from config.pri instead:
      open( CONFIG, "<", File::Spec->catfile( $sourceDir, "..", "..", "config.pri" ) )
          || die( $! );
      $openTodoListVersion = undef;
      while ( <CONFIG> ) {
          if ( $_ =~ m/^\s*VERSION\s*=\s*([^\s]+)\s*$/ ) {
              $openTodoListVersion = $1;
              last;
          }
      }
      close( CONFIG );
      if ( !$openTodoListVersion ) {
          die( "Unable to get OpenTodoList version!" );
      }
  }
}

# Prepare typical input arguments
my $configFile = File::Spec->catfile( $tempDir, "config", "config.xml" );
my $packagesDir = File::Spec->catfile( $tempDir, "packages" );
my $exeSuffix = $Config{_exe};
my $onlineRepositoryUrl = "http://www.rpdev.net/public/repositories/qtifw/OpenTodoList-$os-$arch";

# Get current date and format for use in installer's config files
printf( "Generating release date string...\n" );
my $releaseDate = strftime( '%Y-%m-%d', localtime( time ) );

# Create directory structure in temporary directory
mkdir( File::Spec->catfile( $tempDir, "config" ) );
mkdir( File::Spec->catfile( $tempDir, "packages" ) );
mkdir( File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList" ) );
mkdir( File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList", "meta" ) );
mkdir( File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList", "data" ) );

# Copy over installer config file
createTargetConfig(
    File::Spec->catfile( $sourceDir, "config", "config.xml" ),
    File::Spec->catfile( $tempDir, "config", "config.xml" ),
    { '\$Version' => $openTodoListVersion,
      '\$ReleaseDate' => $releaseDate,
      '\$RepositoryUrl' => $onlineRepositoryUrl } );

# Copy license files:
copy( File::Spec->catfile( $sourceDir, "..", "..", "COPYING" ),
      File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList", "meta", "COPYING" ) );

# Copy script files:
copy( File::Spec->catfile( $sourceDir, "packages", "net.rpdev.OpenTodoList", "meta", "script.js" ),
      File::Spec->catfile( $tempDir,   "packages", "net.rpdev.OpenTodoList", "meta", "script.js" ) );

# Copy icon files:
copy( File::Spec->catfile( $sourceDir, "..", "..", "OpenTodoList", "OpenTodoList.icns" ),
      File::Spec->catfile( $tempDir, "config", "OpenTodoList.icns" ) );
copy( File::Spec->catfile( $sourceDir, "..", "..", "OpenTodoList", "OpenTodoList.ico" ),
      File::Spec->catfile( $tempDir, "config", "OpenTodoList.ico" ) );
copy( File::Spec->catfile( $sourceDir, "..", "..", "OpenTodoList", "icons", "OpenTodoList80.png" ),
      File::Spec->catfile( $tempDir, "config", "OpenTodoList.png" ) );

# Copy over package file, inserting placeholders
createTargetConfig(
    File::Spec->catfile( $sourceDir, "packages", "net.rpdev.OpenTodoList", "meta", "package.xml" ),
    File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList", "meta", "package.xml" ),
    { '\$Version' => $openTodoListVersion,
      '\$ReleaseDate' => $releaseDate } );

# Create data file
printf( "Creating OpenTodoList core package...\n" );
my $openTodoListDataFile = File::Spec->catfile( $tempDir, "packages", "net.rpdev.OpenTodoList",
  "data", "OpenTodoList.7z" );
my $inputDir = File::Spec->catfile( $installDir, "OpenTodoList" );
#Check if we are on Mac OS and apply fix:
if ( ! -d $inputDir && -f File::Spec->catfile( $installDir, "Contents", "MacOS", "OpenTodoList" ) ) {
    $inputDir = $installDir;
}
qx(archivegen "$openTodoListDataFile" "$inputDir");

# Create offline installer
printf( "Creating offline installer...\n" );
my $offlineOnlyArg = $offlineOnly ? "--offline-only" : "";
my $offlineInstallerBinary = File::Spec->catfile( $targetDir, "OpenTodoList-offline-$os-$arch-$openTodoListVersion$exeSuffix" );
qx(binarycreator -c $configFile -p $packagesDir $offlineOnlyArg $offlineInstallerBinary);

# Create Repository
printf( "Creating repository...\n" );
my $onlineRepositoryDir = File::Spec->catfile( $targetDir, "OpenTodoList-$os-$arch" );
qx(repogen -c $configFile -p $packagesDir $onlineRepositoryDir);

# Create online installer
printf( "Creating online only installer...\n" );
my $onlineInstallerBinary = File::Spec->catfile( $targetDir, "OpenTodoList-online-$os-$arch-$openTodoListVersion$exeSuffix" );
my $updateArg = $update ? "-u $onlineRepositoryUrl" : "";
qx(binarycreator -c $configFile -p $packagesDir --online-only $updateArg $onlineInstallerBinary);

if ( $copyQtMenuNib ) {
    printf( "Copying qt_menu.nib...\n" );
    my $binaryCreatorBin = dirname( `which binarycreator` );
    my $qtMenuNib = File::Spec->catfile( $binaryCreatorBin,
     "..", "Uninstaller.app", "Contents", "Resources", "qt_menu.nib" );

    `cp -a $qtMenuNib $offlineInstallerBinary.app/Contents/Resources`;
    `cp -a $qtMenuNib $onlineInstallerBinary.app/Contents/Resources`;
}

if ( $dmg ) {
    printf( "Creating MacOS Disk Images...\n" );
    my $offlineInstallerBaseName = basename($offlineInstallerBinary);
    my $onlineInstallerBaseName = basename($onlineInstallerBinary);
    my $offlineInstallerDmg = File::Spec->catfile( $targetDir, "OpenTodoList-offline-$os-$arch-$openTodoListVersion.dmg" );
    my $onlineInstallerDmg = File::Spec->catfile( $targetDir, "OpenTodoList-online-$os-$arch-$openTodoListVersion.dmg" );
    `mkdir $tempDir/$offlineInstallerBaseName`;
    `cp -R $offlineInstallerBinary.app/ $tempDir/$offlineInstallerBaseName`;
    `hdiutil create $offlineInstallerDmg -srcfolder $tempDir/$offlineInstallerBaseName`;
    `mkdir $tempDir/$onlineInstallerBaseName`;
    `cp -R $onlineInstallerBinary.app/ $tempDir/$onlineInstallerBaseName`;
    `hdiutil create $onlineInstallerDmg -srcfolder $tempDir/$onlineInstallerBaseName`;
}
