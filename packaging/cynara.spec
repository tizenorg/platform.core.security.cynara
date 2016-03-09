Name:       libcynara-base
Summary:    Cynara base libraries
Version:    0.10.0
Release:    1
Group:      Security/Application Privilege
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
BuildRequires: cmake
BuildRequires: zip

%description
Base package.

%package -n libcynara-base-devel
Summary:    Cynara development files
Requires:   libcynara-base = %{version}-%{release}

%description -n libcynara-base-devel
Cynara development files

%prep
%setup -q
cp -a %{SOURCE1001} .

%build

export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . \
        -DBUILD_TESTS=ON \
        -DBUILD_BASE=ON \
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

%post -n libcynara-base -p /sbin/ldconfig

%postun -n libcynara-base -p /sbin/ldconfig

%files -n libcynara-base-devel
%{_includedir}/*.h
%{_libdir}/pkgconfig/cynara-base.pc
%{_libdir}/*.so

%files -n libcynara-base
%manifest cynara.manifest
%license LICENSE
%{_libdir}/*.so.*

