#!/usr/bin/perl

=head1 SYNOPSIS

linux-deploy-qt.pl OPTIONS

=head1 DESCRIPTION

Deploys required Qt libraries into the OpenTodoList
installation directory.

=cut

use strict;
use warnings;
use utf8;

use File::Basename;
use File::Copy qw( copy );
use File::Copy::Recursive qw( rcopy );
use File::Find;
use File::Spec;
use Getopt::Long;
use Pod::Usage;

=over 8

=item --executable EXE

The OpenTodoList executable file.

=cut

my $exe;

=item --help

Shows the build in help.

=cut

my $help;

=item --man

Shows the scripts manual page.

=cut

my $man;

=item --qt QTDIR

The path to the Qt installation (from which to pull libraries).

=cut

my $qtDir;

=back

=cut

my $argParseError;

GetOptions(
            "exe=s" => \$exe,
            "help" => \$help,
            "man" => \$man,
            "qt=s" => \$qtDir,
            )
|| die( "Invalid arguments passed!" );

pod2usage( -exitval => 0, -verbose => 1 ) if $help;
pod2usage( -exitVal => 0, -verbose => 2 ) if $man;

$argParseError = "No exe specified!" if !defined( $exe );
$argParseError = "No qt specified!" if !defined( $qtDir );

if ( defined($argParseError) ) {
    pod2usage( -exitval => 1,
               -verbose => 1,
               -msg => $argParseError );
}

# Prepare some variable we need later on:
my $binDir = dirname( $exe );
my $libDir = File::Spec->catfile( $binDir, "..", "lib" );
my $qmlDir = File::Spec->catfile( $binDir, "..", "qml" );
my $pluginsDir = File::Spec->catfile( $binDir, "..", "plugins" );

# Create directories:
mkdir( $libDir );
mkdir( $qmlDir );
mkdir( $pluginsDir );

# Change rpath of executable:
qx( patchelf --set-rpath '\$ORIGIN/../lib' $exe );

# Create qt.conf file:
open( CONF, ">", File::Spec->catfile( $binDir, "qt.conf" ) ) || die( $! );
print( CONF "[Paths]\nPrefix=.." );
close( CONF );

# Copy required QML modules:
for my $qmlModule ( "QtQuick", "QtQuick.2", "QtGraphicalEffects" ) {
    rcopy(
        File::Spec->catfile( $qtDir, "qml", $qmlModule ),
        File::Spec->catfile( $qmlDir, $qmlModule ) );
}

# Copy required plugins:
for my $pluginType ( "generic", "imageformats", "platforminputcontexts", "platforms", "sqldrivers" ) {
    rcopy(
        File::Spec->catfile( $qtDir, "plugins", $pluginType ),
        File::Spec->catfile( $pluginsDir, $pluginType ) );
}

# Copy translations:
rcopy( File::Spec->catfile( $qtDir, "translations" ),
       File::Spec->catfile( $binDir, "..", "translations" ) );

# Find dependent Qt libraries and copy them as well:
my @queue = ($exe);
my %handledDeps = ();
find( sub {
    if ( -f $File::Find::name && $File::Find::name =~ m/.so$/ ) {
        push( @queue, $File::Find::name );
    }
}, ( $pluginsDir, $qmlDir ) );
while ( $#queue >= 0 ) {
    my $currentObj = pop( @queue );
    if ( !$handledDeps{$currentObj} ) {
        $handledDeps{$currentObj} = 1;
        print( "Copying dependencies of $currentObj\n" );
        for my $line ( qx( ldd $currentObj 2>/dev/null ) ) {
            if ( $line =~ m/(lib[^.]+\.so(\.[^\s]+)?)/ ) {
                my $srcLib = File::Spec->catfile( $qtDir, "lib", $1 );
                if ( -f $srcLib ) {
                    my $dstLib = File::Spec->catfile( $libDir, $1 );
                    if ( ! -f $dstLib ) {
                        copy( $srcLib, $dstLib );
                        qx( patchelf --set-rpath '\$ORIGIN' $dstLib );
                    }
                    push( @queue, $dstLib );
                }
            }
        }
    }
}
