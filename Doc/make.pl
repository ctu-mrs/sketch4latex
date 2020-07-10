#
# make sketch documentation with Windows and Cygwin
#
use strict;
use File::Copy;

our $WIN32 = ($^O =~ /win32/i);

sub make_example {
  my $ex = shift;
  die "missing example name" unless $ex;
  my %arg = map { $_ => 1 } @_;
  my ($GS, $SKETCH, $LATEX_OPTS);
  if ($WIN32) {
    $GS = 'gswin32.exe';
    $SKETCH = -x '..\sketch.exe' ? '..\sketch.exe' : 'sketch.exe';
    # $LATEX_OPTS = ""; # MikTeX 2.6
    $LATEX_OPTS = '--extra-mem-top=5000000'; # MikTeX 2.7
  }
  else {
    $GS = 'gs';
    $SKETCH = -x '../sketch' ? '../sketch' : 'sketch';
    $LATEX_OPTS = '';
  }

  unless ($arg{nosketch}) {
    my $cmd = "$SKETCH -D $ex manexamples.sk > $ex.tex";
    print STDERR "$cmd\n";
    system($cmd) == 0 or die "$!";
  }
  unless ($arg{nobitmaps}) {
    print STDERR "latex example '$ex-tmp.tex':\n";
    system("sed -e s/TEXFILE/$ex/ makeex-tmp.tex > $ex-tmp.tex") == 0 or die "$!";
    system("latex $LATEX_OPTS $ex-tmp.tex") == 0 or die "$!";
    system("dvips -E $ex-tmp -o tmp.eps") == 0 or die "$!";
    # fix up bounding box (originally this was not necessary; something was "improved")
    system("epstool --copy --bbox tmp.eps $ex.eps") == 0 or die "$!";
    unlink "tmp.eps";
    local *F;
    open(F, "> $ex.txt") or die "$!";
    print F "Image $ex omitted in text version of this document.";
    close F;
    system("$GS -q -dQUIET -dNOPAUSE -dBATCH -dEPSCrop -sDEVICE=pdfwrite -sOutputFile=$ex.pdf -r120 $ex.eps") == 0 or die "$!";
    system("$GS -q -dQUIET -dNOPAUSE -dBATCH -dEPSCrop -sDEVICE=png256   -sOutputFile=$ex.png -r120 $ex.eps") == 0 or die "$!";
    unlink <$ex-tmp.*>;
  }
}

sub make_all {
  my %arg = map { $_ => 1 } @_;
  if ($arg{examples}) {
    local *F;
    open(F, "tags.sk") or die "$!";
    while (<F>) {
      if (/<([^>]*)>/) {
        make_example $1, @_;
      }
    }
    close(F);
  }
  # Delete spurious carriage returns in unix/linux.
  unless ($WIN32){ 
    rename("sketch.texi","sketch.texi.dist") || die "$!"; 
    open (IN,"<sketch.texi.dist") || die "$!"; 
    open (OUT,">sketch.texi") || die "$!"; 
    while(<IN>){
      tr/\r//d;
      print OUT;
    }
    close OUT;
  }
  unless ($arg{notypeset}) {
    print "info:\n";
    system("makeinfo sketch.texi") == 0 or die "$!";

    print "html, one page per node:\n";
    system("makeinfo --html sketch.texi") == 0 or die "$!";
    while (<*.png>) { 
      copy($_, "sketch/$_");
    }

    print "html, single page:\n";
    system("makeinfo --html --no-split sketch.texi") == 0 or die "$!";

    # run twice to ensure labels are ok
    print "TeX DVI\n";
    system("texi2dvi sketch.texi") == 0 or die "$!";
    system("texi2dvi sketch.texi") == 0 or die "$!";

    print "Postscript:\n";
    system("dvips sketch.dvi -o sketch.ps") == 0 or die "$!";

    # texi2dvi finds PNG before the PDF graphics, so delete...
    my @unlink_list = <ex???.png>;
    unlink @unlink_list;

    # make PDF manual
    print "PDF:\n";
    system("texi2dvi --pdf sketch.texi") == 0 or die "$!";

    # ... and restore
    for (@unlink_list) {
      copy("sketch/$_", $_);
    }
  }
  # Restor the distibution version of the texi file if we rewrote it.
  unless ($WIN32){ 
    unlink "sketch.texi" || die "$!"; 
    rename "sketch.texi.dist","sketch.texi" || die "$!"; 
  }
}

if (grep { /help/ } @ARGV) {
  print "usage: perl make.pl [examples] [nosketch] [nobitmaps] [notypeset]\n";
}
elsif ($ARGV[0] =~ /^ex[0-9]+/ ) {
  make_example @ARGV
}
else {
  make_all @ARGV
}
