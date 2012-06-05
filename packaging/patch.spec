Name:       patch
Summary:    The GNU patch command, for modifying/upgrading files
Version:    2.6
Release:    1
Group:      Development/Tools
License:    GPLv3
URL:        http://www.gnu.org/software/patch/patch.html
Source0:    ftp://ftp.gnu.org/gnu/patch/patch-%{version}.tar.xz
Source1001: packaging/patch.manifest 
Patch0:     patch-2.5.4-sigsegv.patch


%description
The patch program applies diff files to originals.  The diff command
is used to compare an original to a changed file.  Diff lists the
changes made to the file.  A person who has the original file can then
use the patch command with the diff file to add the changes to their
original file (patching the file).

Patch should be installed because it is a common way of upgrading
applications.


%prep
%setup -q -n %{name}-%{version}

# patch-2.5.4-sigsegv.patch
%patch0 -p1

%build
cp %{SOURCE1001} .
CFLAGS="$RPM_OPT_FLAGS -D_GNU_SOURCE"

%configure --disable-static
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%manifest patch.manifest
%defattr(-,root,root,-)
%{_bindir}/*
%doc %{_mandir}/*/*
