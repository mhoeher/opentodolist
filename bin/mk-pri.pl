#!/usr/bin/perl

use common::sense;
use Pod::Usage;
use Getopt::Long;
use Cwd;
use File::Find;
use File::Spec;
use File::Basename;

my $dir = getcwd();
my $pri;
my $base;
my $help;

GetOptions( "dir=s" => \$dir,
            "output=s" => \$pri,
            "base=s" => \$base,
            "help" => \$help ) || pod2usage( -exit => 2, -verbose => 1 );

pod2usage( -exit => 1, -verbose => 2 ) if $help;

if ( !$base ) {
	$base = $dir;
}

$dir = File::Spec->rel2abs( $dir );

my $file;
if ( $pri ) {
    open( $file, ">", $pri ) || die( "Unable to open $pri: $!\n" );
} else {
    $file = *STDOUT;
}

print $file "OTHER_FILES += ";

find( sub { if ( -f $File::Find::name ) {
            my $relPath = File::Spec->abs2rel( $File::Find::name, dirname( $base ) );
            print $file "\\\n  $relPath";
} }, $dir );

print $file "\n";

if ( $pri ) {
    close( $file );
}

=head1 SYNOPSIS

mk-pri.pl

=head1 OPTIONS

=over 4

=item B<--dir DIR> Search DIR recursively for files

=item B<--output FILE> Write output to FILE

=item B<--base DIR> Generate file paths in output file relative to DIR.

Omitting this option will cause paths to be relative to the location specified by the
B<--dir> option.

=item B<--help> Show help

=back

=head1 DESCRIPTION

This script searches a given input directory recursively
for any file and writes everything found into a *.pri (Qt Project Include) file.
The *.pri file is a simple project file that usually is included in
other *.pro/*.pri files.

=cut
