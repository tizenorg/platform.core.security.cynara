Name:       cynara
Summary:    Cynara service with client libraries
Version:    0.2.2
Release:    1
Group:      Security/Access Control
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001:    cynara.manifest
Source1002:    libcynara-client.manifest
Source1003:    libcynara-admin.manifest
Source1004:    cynara-tests.manifest
Source1005:    libcynara-helper-credentials.manifest
Source1006:    libcynara-helper-credentials-dbus.manifest
Source1007:    libcynara-helper-credentials-socket.manifest
Source1008:    libcynara-helper-session.manifest
Requires:      default-ac-domains
BuildRequires: cmake
BuildRequires: zip
BuildRequires: pkgconfig(libsystemd-daemon)
BuildRequires: pkgconfig(libsystemd-journal)
%{?systemd_requires}

%global user_name %{name}
%global group_name %{name}

%global state_path %{_localstatedir}/%{name}/

%global build_type %{?build_type:%build_type}%{!?build_type:RELEASE}

%if %{?build_type} == "DEBUG"

BuildRequires: pkgconfig(libunwind)

%endif

%description
service, client libraries (libcynara-client, libcynara-admin),
helper libraries (libcynara-helper-session, libcynara-helper-credentials,
libcynara-helper-credentials-dbus, libcynara-helper-credentials-socket)
and tests (cynara-tests)

#######################################################
%package -n libcynara-client
Summary:    Cynara - client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-client
client library for checking policies

%package -n libcynara-client-devel
Summary:    Cynara - client library (devel)
Requires:   libcynara-client = %{version}-%{release}

%description -n libcynara-client-devel
client library (devel) for checking policies

#######################################################
%package -n libcynara-admin
Summary:    Cynara - admin client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-admin
admin client library for setting, listing and removing policies

%package -n libcynara-admin-devel
Summary:    Cynara - admin client library (devel)
Requires:   libcynara-admin = %{version}-%{release}

%description -n libcynara-admin-devel
admin client library (devel) for setting, listing and removing policies

#######################################################
%package -n libcynara-helper-credentials
Summary:    Base library for cynara credentials helpers
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-credentials
Base library for cynara credentials helpers

%package -n libcynara-helper-credentials-devel
Summary:    Base library for cynara credentials helpers (devel)
Requires:   libcynara-helper-credentials = %{version}-%{release}

%description -n libcynara-helper-credentials-devel
Base library for cynara credentials helpers (devel)

#######################################################
%package -n libcynara-helper-credentials-dbus
Summary:    Cynara credentials helpers library for dbus clients
BuildRequires: dbus-1
BuildRequires: pkgconfig(dbus-glib-1)
Requires:   dbus-1
Requires:   pkgconfig(dbus-glib-1)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-credentials-dbus
Cynara credentials helpers library for dbus clients

%package -n libcynara-helper-credentials-dbus-devel
Summary:    Cynara credentials helpers library for dbus clients (devel)
Requires:   libcynara-helper-credentials-dbus = %{version}-%{release}

%description -n libcynara-helper-credentials-dbus-devel
Cynara credentials helpers library for dbus clients (devel)

#######################################################
%package -n libcynara-helper-credentials-socket
Summary:    Cynara credentials helpers library for socket clients
BuildRequires: pkgconfig(libsmack)
Requires:   smack
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-credentials-socket
Cynara credentials helpers library for socket clients

%package -n libcynara-helper-credentials-socket-devel
Summary:    Cynara credentials helpers library for socket clients (devel)
Requires:   libcynara-helper-credentials-socket = %{version}-%{release}

%description -n libcynara-helper-credentials-socket-devel
Cynara credentials helpers library for socket clients (devel)

#######################################################
%package -n libcynara-helper-session
Summary:    Cynara helper client session string creation library
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-session
Cynara helper client session string creation library

%package -n libcynara-helper-session-devel
Summary:    Cynara helper client session string creation library (devel)
Requires:   libcynara-helper-session = %{version}-%{release}

%description -n libcynara-helper-session-devel
Cynara helper client session string creation library (devel)

#######################################################
%package -n cynara-tests
Summary:    Cynara - cynara test binaries
BuildRequires: pkgconfig(gmock)

%description -n cynara-tests
Cynara tests

#######################################################

%package -n cynara-devel
Summary:    Cynara service (devel)
Requires:   cynara = %{version}-%{release}

%description -n cynara-devel
service (devel version)

%prep
%setup -q
cp -a %{SOURCE1001} .
cp -a %{SOURCE1002} .
cp -a %{SOURCE1003} .
cp -a %{SOURCE1004} .
cp -a %{SOURCE1005} .
cp -a %{SOURCE1006} .
cp -a %{SOURCE1007} .
cp -a %{SOURCE1008} .

%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

export CXXFLAGS="$CXXFLAGS -DCYNARA_STATE_PATH=\\\"%{state_path}\\\""
export LDFLAGS+="-Wl,--rpath=%{_libdir}"

%cmake . -DVERSION=%{version} \
        -DCMAKE_BUILD_TYPE=%{?build_type} \
        -DCMAKE_VERBOSE_MAKEFILE=ON
make %{?jobs:-j%jobs}

%install

rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/lib/systemd/system/sockets.target.wants
mkdir -p %{buildroot}/%{state_path}
ln -s ../cynara.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara.socket
ln -s ../cynara-admin.socket %{buildroot}/usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket

%pre
id -g %{group_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    groupadd %{group_name} -r > /dev/null 2>&1
fi

id -u %{user_name} > /dev/null 2>&1
if [ $? -eq 1 ]; then
    useradd -m %{user_name} -r > /dev/null 2>&1
fi

%post
### Add file capabilities if needed
### setcap/getcap binary are useful. To use them you must install libcap and libcap-tools packages
### In such case uncomment Requires with those packages

systemctl daemon-reload

if [ $1 = 1 ]; then
    systemctl enable %{name}.service
fi

chsmack -a System %{state_path}

systemctl restart %{name}.service

/sbin/ldconfig

%preun
if [ $1 = 0 ]; then
    # unistall
    systemctl stop cynara.service
fi

%postun
if [ $1 = 0 ]; then
    userdel -r %{user_name} > /dev/null 2>&1
    groupdel %{user_name} > /dev/null 2>&1
    systemctl daemon-reload
fi

/sbin/ldconfig

%post -n libcynara-client -p /sbin/ldconfig

%postun -n libcynara-client -p /sbin/ldconfig

%post -n libcynara-admin -p /sbin/ldconfig

%postun -n libcynara-admin -p /sbin/ldconfig

%post -n libcynara-client-devel -p /sbin/ldconfig

%postun -n libcynara-client-devel -p /sbin/ldconfig

%post -n libcynara-admin-devel -p /sbin/ldconfig

%postun -n libcynara-admin-devel -p /sbin/ldconfig

%post -n libcynara-helper-credentials -p /sbin/ldconfig

%postun -n libcynara-helper-credentials -p /sbin/ldconfig

%post -n libcynara-helper-credentials-devel -p /sbin/ldconfig

%postun -n libcynara-helper-credentials-devel -p /sbin/ldconfig

%post -n libcynara-helper-credentials-dbus -p /sbin/ldconfig

%postun -n libcynara-helper-credentials-dbus -p /sbin/ldconfig

%post -n libcynara-helper-credentials-dbus-devel -p /sbin/ldconfig

%postun -n libcynara-helper-credentials-dbus-devel -p /sbin/ldconfig

%post -n libcynara-helper-credentials-socket -p /sbin/ldconfig

%postun -n libcynara-helper-credentials-socket -p /sbin/ldconfig

%post -n libcynara-helper-credentials-socket-devel -p /sbin/ldconfig

%postun -n libcynara-helper-credentials-socket-devel -p /sbin/ldconfig

%post -n libcynara-helper-session -p /sbin/ldconfig

%postun -n libcynara-helper-session -p /sbin/ldconfig

%post -n libcynara-helper-session-devel -p /sbin/ldconfig

%postun -n libcynara-helper-session-devel -p /sbin/ldconfig

%files -n cynara
%manifest cynara.manifest
%license LICENSE
%attr(755,root,root) /usr/bin/cynara
%{_libdir}/libcynara-commons.so*
%attr(-,root,root) /usr/lib/systemd/system/cynara.service
%attr(-,root,root) /usr/lib/systemd/system/cynara.target
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara.socket
%attr(-,root,root) /usr/lib/systemd/system/sockets.target.wants/cynara-admin.socket
%attr(-,root,root) /usr/lib/systemd/system/cynara-admin.socket
%dir %attr(700,cynara,cynara) %{state_path}

%files -n libcynara-client
%manifest libcynara-client.manifest
%license LICENSE
%defattr(-,root,root,-)
%{_libdir}/libcynara-client.so.*

%files -n libcynara-client-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-client.h
%{_includedir}/cynara/cynara-client-error.h
%{_libdir}/pkgconfig/cynara-client.pc
%{_libdir}/libcynara-client.so

%files -n libcynara-admin
%manifest libcynara-admin.manifest
%license LICENSE
%defattr(-,root,root,-)
%{_libdir}/libcynara-admin.so.*

%files -n libcynara-admin-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-admin.h
%{_includedir}/cynara/cynara-admin-error.h
%{_libdir}/libcynara-admin.so
%{_libdir}/pkgconfig/cynara-admin.pc

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests

%files -n libcynara-helper-credentials
%manifest libcynara-helper-credentials.manifest
%license LICENSE
%{_libdir}/libcynara-helper-credentials.so.*

%files -n libcynara-helper-credentials-devel
%{_includedir}/cynara/cynara-helper-credentials.h
%{_libdir}/libcynara-helper-credentials.so
%{_libdir}/pkgconfig/cynara-helper-credentials.pc

%files -n libcynara-helper-credentials-dbus
%manifest libcynara-helper-credentials-dbus.manifest
%license LICENSE
%{_libdir}/libcynara-helper-credentials-dbus.so.*

%files -n libcynara-helper-credentials-dbus-devel
%{_includedir}/cynara/cynara-helper-credentials-dbus.h
%{_libdir}/libcynara-helper-credentials-dbus.so
%{_libdir}/pkgconfig/cynara-helper-credentials-dbus.pc

%files -n libcynara-helper-credentials-socket
%manifest libcynara-helper-credentials-socket.manifest
%license LICENSE
%{_libdir}/libcynara-helper-credentials-socket.so.*

%files -n libcynara-helper-credentials-socket-devel
%{_includedir}/cynara/cynara-helper-credentials-socket.h
%{_libdir}/libcynara-helper-credentials-socket.so
%{_libdir}/pkgconfig/cynara-helper-credentials-socket.pc

%files -n libcynara-helper-session
%manifest libcynara-helper-session.manifest
%license LICENSE
%{_libdir}/libcynara-helper-session.so.*

%files -n libcynara-helper-session-devel
%{_includedir}/cynara/cynara-helper-session.h
%{_libdir}/libcynara-helper-session.so
%{_libdir}/pkgconfig/cynara-helper-session.pc
