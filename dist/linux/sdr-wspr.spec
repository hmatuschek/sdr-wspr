Summary: A WSPR recevier using libsdr.

%define version 0.1.0

License: GPL-2.0+
Name: sdr-wspr
Group: Applications/Communications
Prefix: /usr
Release: 1
Source: sdr-wspr-%{version}.tar.gz
URL: https://github.com/hmatuschek/sdr-wspr
Version: %{version}
Buildroot: /tmp/sdrwsprrpm
BuildRequires: gcc-c++, gcc-fortran, cmake, portaudio-devel, fftw3-devel, rtl-sdr-devel, libsdr-devel
Requires: portaudio, fftw3, rtl-sdr, libsdr
%if 0%{?suse_version}
BuildRequires: libqt5-qtbase-devel, libQt5WebKit5-devel, libQt5WebKitWidgets-devel, alsa-devel
Requires: libqt5-qtbase
%endif
%if 0%{?fedora}
BuildRequires: qt5-qtbase-devel, qt5-qtwebkit-devel
Requires: qt5-qtbase, qt5-qtwebkit
%endif

%description
A simple WSPR receiver using libsdr.

%prep
%setup -q

%build
cmake -DCMAKE_BUILD_TYPE=RELEASE \
      -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/usr
make

%install
make install

%clean
rm -rf $RPM_BUILD_ROOT

 
%files
%defattr(-, root, root, -)
%attr(755, root, root) %{_prefix}/bin/sdr-wspr
%{_prefix}/share/applications/sdr-wspr.desktop
%{_prefix}/share/icons/sdr-wspr.svg

