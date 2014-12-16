#!/usr/bin/perl

use common::sense;
use XML::LibXML;
use Pod::Usage;
use Getopt::Long;
use Cwd;
use File::Find;
use File::Spec;
use File::Basename;

sub addDir {
  my ( $dir, $base, $root, $doc ) = @_;
  opendir( DIR, $dir ) || die($!);
  my @dirs = ();
  my @files = ();
  while ( my $entry = readdir( DIR ) ) {
    my $absName = File::Spec->catfile( $dir, $entry );
    if ( not ( $entry eq "." || $entry eq ".." ) ) {
      push( @dirs, $absName ) if ( -d $absName );
      push( @files, $absName ) if ( -f $absName );
    }
  }
  closedir(DIR);
  if ( scalar(@files) > 0 ) {
    my $prefix = $doc->createElement( "qresource" );
    my $prefixName = "/" . File::Spec->abs2rel( $dir, $base );
    $prefixName =~ s/\\/\//g;
    $root->appendChild( $prefix );
    $prefix->setAttribute( "prefix", $prefixName );
    for my $f ( @files ) {
      my $file = $doc->createElement( "file" );
      $file->setAttribute( "alias", basename( $f ) );
      $file->appendText( File::Spec->abs2rel( $f, $base ) );
      $prefix->appendChild( $file );
    }
  }
  for my $d ( @dirs ) {
    addDir( $d, $base, $root, $doc );
  }

}

my $dir = getcwd();
my $qrc;
my $prefix = "/";
my $base;
my $help;

GetOptions( "dir=s" => \$dir,
            "output=s" => \$qrc,
            "base=s" => \$base,
            "help" => \$help ) || pod2usage( -exit => 2, -verbose => 1 );

pod2usage( -exit => 1, -verbose => 2 ) if $help;

if ( ! $base ) {
    $base = $dir;
}

$dir = File::Spec->rel2abs( $dir );

my $doc = XML::LibXML::Document->new();
my $root = $doc->createElement( "RCC" );
$doc->setDocumentElement( $root );

addDir( $dir, $base, $root, $doc );

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

=item B<--prefix PREFIX> Put all found resources under PREFIX

=item B<--base DIR> Make paths relative to DIR

If this option is omitted, then instead paths will be relative to
the location pointed to by B<--dir>.

=item B<--help> Show help

=back

=head1 DESCRIPTION

This script searches a given input directory recursively
for any file and writes everything found into a *.qrc (Qt Resource) file.
The *.qrc is an XML file which is compiled by Qt.

=cut
