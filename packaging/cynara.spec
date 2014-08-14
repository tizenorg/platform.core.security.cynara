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
Source1005:    libcynara-helper-session.manifest
Source1006:    libcynara-helper-socket-credentials.manifest
Source1007:    libcynara-helper-dbus-credentials.manifest
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
Service, client libraries (libcynara-client, libcynara-admin), 
helper libraries (libcynara-helper-session, libcynara-helper-socket, 
libcynara-helper-dbus) and tests (cynara-tests)

#######################################################
%package -n libcynara-client
Summary:    Cynara - client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-client
Client library for checking policies

%package -n libcynara-client-devel
Summary:    Cynara - client library (devel)
Requires:   libcynara-client = %{version}-%{release}

%description -n libcynara-client-devel
Client library (devel) for checking policies

#######################################################
%package -n libcynara-admin
Summary:    Cynara - admin client library
Requires:   cynara = %{version}-%{release}
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-admin
Admin client library for setting, listing and removing policies

%package -n libcynara-admin-devel
Summary:    Cynara - admin client library (devel)
Requires:   libcynara-admin = %{version}-%{release}

%description -n libcynara-admin-devel
Admin client library (devel) for setting, listing and removing policies

#######################################################
%package -n libcynara-helper-session
Summary:    Helper client session string creation library
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-session
Helper client session string creation library

%package -n libcynara-helper-session-devel
Summary:    Helper client session string creation library (devel)
Requires:   libcynara-helper-session = %{version}-%{release}

%description -n libcynara-helper-session-devel
Helper client session string creation library (devel)

#######################################################
%package -n libcynara-helper-socket-credentials
Summary:    Helper credentials socket library
BuildRequires: pkgconfig(libsmack)
Requires:   smack
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-socket-credentials
Helper credential socket library

%package -n libcynara-helper-socket-credentials-devel
Summary:    Helper credential socket library (devel)
Requires:   libcynara-helper-socket-credentials = %{version}-%{release}

%description -n libcynara-helper-socket-credentials-devel
Helper socket credentials library (devel)

#######################################################
%package -n libcynara-helper-dbus-credentials
Summary:    Helper credentials dbus library
BuildRequires: dbus-1
Requires:   dbus-1 
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description -n libcynara-helper-dbus-credentials
Helper credential dbus library

%package -n libcynara-helper-dbus-credentials-devel
Summary:    Helper credential dbus library (devel)
Requires:   libhelper-dbus-credential = %{version}-%{release}

%description -n libcynara-helper-dbus-credentials-devel
Helper dbus credentials library (devel)



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

%post -n libcynara-helper-session -p /sbin/ldconfig

%postun -n libcynara-helper-session -p /sbin/ldconfig

%post -n libcynara-helper-session-devel -p /sbin/ldconfig

%postun -n libcynara-helper-session-devel -p /sbin/ldconfig

%post -n libcynara-helper-socket-credentials -p /sbin/ldconfig

%postun -n libcynara-helper-socket-credentials -p /sbin/ldconfig

%post -n libcynara-helper-socket-credentials-devel -p /sbin/ldconfig

%postun -n libcynara-helper-socket-credentials-devel -p /sbin/ldconfig

%post -n libcynara-helper-dbus-credentials -p /sbin/ldconfig

%postun -n libcynara-helper-dbus-credentials -p /sbin/ldconfig

%post -n libcynara-helper-dbus-credentials-devel -p /sbin/ldconfig

%postun -n libcynara-helper-dbus-credentials-devel -p /sbin/ldconfig

%files -n cynara
%manifest cynara.manifest
%attr(755,root,root) /usr/bin/cynara
%license LICENSE
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
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libcynara-client.so.*

%files -n libcynara-client-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-client.h
%{_libdir}/pkgconfig/cynara-client.pc
%{_libdir}/libcynara-client.so

%files -n libcynara-admin
%manifest libcynara-admin.manifest
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libcynara-admin.so.*

%files -n libcynara-admin-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/cynara-admin.h
%{_libdir}/libcynara-admin.so
%{_libdir}/pkgconfig/cynara-admin.pc

%files -n cynara-tests
%manifest cynara-tests.manifest
%attr(755,root,root) /usr/bin/cynara-tests

%files -n libcynara-helper-session
%manifest libcynara-helper-session.manifest
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libsession.so.*

%files -n libcynara-helper-session-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/helper-session.h
%{_libdir}/pkgconfig/helper-session.pc
%{_libdir}/libsession.so


%files -n libcynara-helper-socket-credentials
%manifest libcynara-helper-socket-credentials.manifest
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libsocket-credentials.*

%files -n libcynara-helper-socket-credentials-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/socket-credentials-helper.h
%{_libdir}/pkgconfig/helper-socket-credentials.pc
%{_libdir}/libsocket-credentials.so

%files -n libcynara-helper-dbus-credentials
%manifest libcynara-helper-dbus-credentials.manifest
%defattr(-,root,root,-)
%license LICENSE
%{_libdir}/libdbus-credentials.*

%files -n libcynara-helper-dbus-credentials-devel
%defattr(-,root,root,-)
%{_includedir}/cynara/dbus-credentials-helper.h
%{_libdir}/pkgconfig/helper-dbus-credentials.pc
%{_libdir}/libdbus-credentials.so

