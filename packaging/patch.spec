Name:           patch
Version:        2.6.1.169
Release:        1
License:        GPL-3.0
Summary:        The GNU patch command, for modifying/upgrading files
Url:            http://www.gnu.org/software/patch/patch.html
Group:          Development/Tools
Source0:        ftp://ftp.gnu.org/gnu/patch/patch-%{version}.tar.xz
Source1001:     patch.manifest
Patch0:         patch-2.5.4-sigsegv.patch

%description
The patch program applies diff files to originals.  The diff command
is used to compare an original to a changed file.  Diff lists the
changes made to the file.  A person who has the original file can then
use the patch command with the diff file to add the changes to their
original file (patching the file).

Patch should be installed because it is a common way of upgrading
applications.

%prep
%setup -q
%patch0 -p1

%build
cp %{SOURCE1001} .
CFLAGS="%{optflags} -D_GNU_SOURCE"

%configure 
make %{?_smp_mflags}

%install
%make_install

%files
%manifest patch.manifest
%{_bindir}/*
%doc %{_mandir}/*/*
