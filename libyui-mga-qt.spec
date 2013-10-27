%define         git          5ded95a
%define         gitdate      20131027
%define         major        5
%define         libname      %mklibname yui %{major}-mga-qt
%define         develname    %mklibname -d yui-mga-qt

Name:           libyui-mga-qt
Version:        1.0.0
Release:        %mkrel -c git%{gitdate} 0
Summary:        UI abstraction library - Qt plugin
License:        LGPLv2+
Group:          System/Libraries
Url:            git@bitbucket.org:_pmat_/libyui-mga-qt.git
Source0:        %{name}-%{version}.tar.bz2

BuildRequires:    png-devel
BuildRequires:    yui-devel
BuildRequires:    pkgconfig(libyui-qt)
BuildRequires:    pkgconfig(libyui-mga)
BuildRequires:    qt4-devel
BuildRequires:    cmake
BuildRequires:    boost-devel
BuildRequires:    doxygen
BuildRequires:    texlive
BuildRequires:    ghostscript
Requires:         libyui
Requires:         libyui-mga
Requires:         libyui-qt
Requires:         python-qt4

%description
%{summary}

#-----------------------------------------------------------------------

%package -n %libname
Summary:        %{summary}
Group:          System/Libraries
Requires:       libyui
Provides:       %{name} = %{version}-%{release}

%description -n %libname
This package contains the library needed to run programs
dynamically linked with libyui-mga-qt.

%files -n %libname
%doc COPYING*
%{_libdir}/yui/lib*.so.*


#-----------------------------------------------------------------------

%package -n %develname
Summary:        %{summary} header files
Group:          Development/KDE and Qt
Requires:       libyui-devel
Requires:       %{name} = %{version}-%{release}


%description -n %develname
This package provides headers files for libyui-mga-qt development.

%files -n %develname
%{_includedir}/yui
%{_libdir}/yui/lib*.so
%{_libdir}/pkgconfig/libyui-mga-qt.pc
%{_libdir}/cmake/libyui-mga-qt
%doc %{_docdir}/libyui-mga-qt%{major}

#-----------------------------------------------------------------------

%prep
%setup -q -n %{name}-%{version}
%apply_patches

%build
./bootstrap.sh
%cmake -DPREFIX=%{_prefix}  \
       -DDOC_DIR=%{_docdir} \
       -DLIB_DIR=%{_lib}    \
       -DINSTALL_DOCS=yes


%make docs

%install
rm -rf %{buildroot}
%makeinstall_std -C build
find "%{buildroot}" -name "*.la" -delete
