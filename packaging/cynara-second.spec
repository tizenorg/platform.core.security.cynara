Name:       cynara-main
Summary:    Cynara service with client libraries
Version:    0.10.0
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
BuildRequires: pkgconfig(cynara-base)
BuildRequires: pkgconfig(key-base)
BuildRequires: cmake
BuildRequires: zip

%description
service, client libraries (libcynara-client, libcynara-admin),
agent library, helper libraries (libcynara-session, libcynara-creds-common, libcynara-creds-dbus,
libcynara-creds-socket) and tests (cynara-tests)

%prep
%setup -q
cp -a %{SOURCE1001} .

%build

%cmake . \
        -DBUILD_TESTS=ON \
        -DBUILD_SERVICE=ON \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON \
        -DLIB_DIR:PATH=%{_libdir} \
        -DBIN_DIR:PATH=%{_bindir} \
        -DSBIN_DIR:PATH=%{_sbindir} \
        -DSYS_CONFIG_DIR:PATH=%{_sysconfdir} \
        -DINCLUDE_DIR:PATH=%{_includedir} \
        -DLOCAL_STATE_DIR:PATH=%{_localstatedir} \
        -DDATA_ROOT_DIR:PATH=%{_datadir} \
        -DSYSTEMD_UNIT_DIR:PATH=%{_unitdir} \
        -DSOCKET_DIR:PATH=/run/%{name} \
        -DDB_FILES_SMACK_LABEL="System"
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%pre

%post

%preun

%postun

%files
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/cynara*

