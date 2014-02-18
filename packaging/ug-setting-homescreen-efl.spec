%bcond_with wayland

Name:       ug-setting-homescreen-efl
Summary:    UI Gadget : setting-homescreen-efl
Version:    0.1.3
Release:    0
Group:      misc
License:    Flora License
Source0:    %{name}-%{version}.tar.gz
Source1001: 	ug-setting-homescreen-efl.manifest

BuildRequires: cmake
BuildRequires: edje-tools
BuildRequires: gettext-tools

BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(vconf)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(pkgmgr-info)

%description
Description: UI Gadget, setting-homescreen-efl

%prep
%setup -q
cp %{SOURCE1001} .

%build

%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}/ug/lib/ \
%if %{with wayland}
         -DWAYLAND_SUPPORT=On
%else
         -DWAYLAND_SUPPORT=Off
%endif

make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
%make_install

%post

%files
%manifest %{name}.manifest
%{_sysconfdir}/smack/accesses2.d/ug.setting-homescreen-efl.include
%{_prefix}/ug/lib/*
%{_prefix}/ug/res/*
