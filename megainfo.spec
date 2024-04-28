%global _udevlibdir %{_prefix}/lib/udev
%global _udevrulesdir %{_prefix}/lib/udev/rules.d
Name:           megainfo
Version:        1.0.0
Release:        1%{?dist}
Summary:        discover megaraid perc vd name name

License:        GPLv2+
URL:            https://github.com/freedge/megainfo/
Source0:        https://github.com/freedge/megainfo/archive/refs/tags/1.0.0.tar.gz

BuildRequires:  make gcc pciutils-devel glibc-static

%description


%prep
%setup -q
cp %{SOURCE0} .


%build
make LDFLAGS="-O2 -static -g" CFLAGS="-O2 -g"


%install
mkdir -p $RPM_BUILD_ROOT%{_udevrulesdir}
install -p -m 644 69-mine.rule $RPM_BUILD_ROOT/%{_udevrulesdir}/69-mine.rules
install -p -m 755 megainfo $RPM_BUILD_ROOT%{_udevlibdir}
install -p -m 755 megainfo.sh $RPM_BUILD_ROOT%{_udevlibdir}


%files
%{_udevrulesdir}/69-mine.rules
%{_udevlibdir}/megainfo
%{_udevlibdir}/megainfo.sh

%changelog
* Sun Apr 28 2024 François Rigault <rigault.francois@gmail.com>
- 
