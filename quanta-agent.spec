Name:	quanta-agent
Version: 1.3.0
Release: 1%{?dist}
Summary: This package provides a daemon to monitor system health and reports to Quanta servers.
Distribution: Quanta
Vendor: Quanta-Computing
Packager: Matthieu ROSINSKI <support@quanta.io>
Group: Applications/Internet
URL: http://www.quanta.io
License: toto

%package -n quanta-agent
Summary: Quanta agent with basic plugins
Group: Applications/Internet
Requires: quanta-agent-base = %{version}
Requires: quanta-agent-cpu = %{version}
Requires: quanta-agent-disk = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-memory = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-process = %{version}
Requires: quanta-agent-system = %{version}


%package -n quanta-agent-all
Summary: Quanta agent with all plugins
Group: Applications/Internet
Requires: quanta-agent-base = %{version}
Requires: quanta-agent-cpu = %{version}
Requires: quanta-agent-disk = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-memory = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-process = %{version}
Requires: quanta-agent-system = %{version}
Requires: quanta-agent-apache = %{version}
Requires: quanta-agent-nginx = %{version}
Requires: quanta-agent-memcached = %{version}
Requires: quanta-agent-redis = %{version}
Requires: quanta-agent-mysql = %{version}
Requires: quanta-agent-postgresql = %{version}
Requires: quanta-agent-varnish = %{version}
Requires: quanta-agent-nfs = %{version}



%description -n quanta-agent
This package provides a daemon to monitor system health and reports to Quanta servers.

%description -n quanta-agent-all
This package provides a daemon to monitor system health and reports to Quanta servers.


%prep
%setup -c -T

%install
