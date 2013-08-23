#!/usr/bin/perl

use common::sense;
use XML::LibXML;
use Pod::Usage;
use Getopt::Long;
use Cwd;
use File::Find;
use File::Spec;
use File::Basename;

my $dir = getcwd();
my $qrc;
my $help;

GetOptions( "dir=s" => \$dir,
            "output=s" => \$qrc,
            "help" => \$help ) || pod2usage( -exit => 2, -verbose => 1 );

pod2usage( -exit => 1, -verbose => 2 ) if $help;

$dir = File::Spec->rel2abs( $dir );

my $doc = XML::LibXML::Document->new();
my $root = $doc->createElement( "RCC" );
$doc->setDocumentElement( $root );
my $resource = $doc->createElement( "qresource" );
$resource->setAttribute( "prefix", "/" );
$root->appendChild( $resource );

find( sub { if ( -f $File::Find::name ) {
            my $file = $doc->createElement( "file" );
            my $relPath = File::Spec->abs2rel( $File::Find::name, dirname( $dir ) );
            $file->appendText( $relPath );
            $resource->appendChild( $file );
} }, $dir );

if ( $qrc ) {
    $doc->toFile( $qrc, 2 );
} else {
    print $doc->toString( 2 );
}

=head1 SYNOPSIS

mk-qrc.pl

=head1 OPTIONS

=over 4

=item B<--dir DIR> Search DIR recursively for files

=item B<--output FILE> Write output to FILE

=item B<--help> Show help

=back

=head1 DESCRIPTION

This script searches a given input directory recursively
for any file and writes everything found into a *.qrc (Qt Resource) file.
The *.qrc is an XML file which is compiled by Qt.

=cut
