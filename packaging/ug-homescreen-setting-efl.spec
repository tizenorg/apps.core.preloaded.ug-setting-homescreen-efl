
Name:       ug-homescreen-setting-efl
Summary:    UI Gadget : homescreen-setting-efl
Version:    0.1.0
Release:    0
Group:      misc
License:    Flora License
Source0:    %{name}-%{version}.tar.gz

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

%description
Description: UI Gadget, homescreen-setting-efl

%prep
%setup -q

%build

cmake . -DCMAKE_INSTALL_PREFIX=%{/usr/ug/lib}/

make %{?jobs:-j%jobs}


%install
rm -rf %{buildroot}
%make_install

%post

%files
%manifest ug-homescreen-setting-efl.manifest
/etc/smack/accesses2.d/ug.setting-homescreen-efl.include
/usr/ug/lib/*
/usr/ug/res/*
