use strict;
use POSIX qw(strftime);
    
sub run {
  local $/ = undef;
  local *F;

  # get raw data
  open(F, "version.dat") || die "can't open version.dat for input";
  my $x = <F>;
  close F;

  # put data pairs in a hash
  my %pairs = $x =~ /([a-z]+)\s*=\s*(\S+)\s*/g;
  die "syntax error in version.dat\n" unless
		defined $pairs{major} && 
    defined $pairs{minor} && 
    defined $pairs{build};

  # update the build number
  $pairs{build}++ unless $pairs{dist} or $ARGV[0] eq 'noinc';

  # get the current time/date
  my $t = time;
  $pairs{time} = $t;
  my $slt = scalar(localtime($t));
  my $nicelt = strftime("%A, %B %d, %Y", localtime($t));

  # print the header
  open(F, "> version.h") || die "can't open version.h for output";
  print F "// Produced by makever.pl.  Don't edit.\n";
  print F "#define VER_MAJOR $pairs{major}\n";
  print F "#define VER_MINOR $pairs{minor}\n";
  print F "#define VER_BUILD $pairs{build}\n";
  print F "#define VER_BUILD_TIME $t\n";
  print F "#ifndef STRINGIFY\n";
  print F "#define ___S(X) #X\n";
  print F "#define STRINGIFY(X) ___S(X)\n";
  print F "#endif\n";
  print F "#define VER_BUILD_TIME_STRING STRINGIFY($slt)\n";
  my $d = $pairs{dist} ? "d" : "";
  print F qq|#define VER_STRING STRINGIFY(VER_MAJOR) "." STRINGIFY(VER_MINOR) " (build " STRINGIFY(VER_BUILD) "$d, " VER_BUILD_TIME_STRING ")"\n|;
  close F;

  # print doc version information
  open(F, "> Doc/version.texi") || die "can't open version.texi";
  print F "\@comment Produced by makever.pl.  Don't edit.\n";
  print F "\@set VER_MAJOR $pairs{major}\n";
  print F "\@set VER_MINOR $pairs{minor}\n";
  print F "\@set VER_BUILD $pairs{build}\n";
  print F "\@set UPDATED $nicelt\n";
  print F "\@set VERSION \@value{VER_MAJOR}.\@value{VER_MINOR} (build \@value{VER_BUILD})\n";
  close F;

  if ( open(F, "index-tmp.htm") ) {
		my $html = <F>;
		close F;
		my $version = "$pairs{major}.$pairs{minor} (build $pairs{build}), $nicelt";
		$html =~ s/VERSION/$version/g;
		my $ver = "$pairs{major}.$pairs{minor}.$pairs{build}";
		$html =~ s/VER/$ver/g;
		open(F, "> index.htm") || die "can't open index.htm for output";
		print F $html;
		close F;
  }
  else {
    warn "not generating web page\n";
  }

  # update the version header
  open(F, "> version.dat") || die "can't open version.dat for output";
  print F join('', map { "$_ = $pairs{$_}\n" } keys %pairs);
  close F;
}

run;
