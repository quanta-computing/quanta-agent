Name:	quanta-agent
Version: 1.0.6
Release: 1%{?dist}
Summary: This package provides a daemon to monitor system health and reports to Quanta servers.
Distribution: Quanta
Vendor: Quanta-Computing
Packager: Matthieu ROSINSKI <support@quanta-computing.com>
Group: Applications/Internet
URL: http://www.quanta-computing.com
License: toto
Requires: quanta-agent-base = %{version}
Requires: quanta-agent-cpu = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-process = %{version}
Requires: quanta-agent-network = %{version}
Requires: quanta-agent-disk = %{version}

%description
This package provides a daemon to monitor system health and reports to Quanta servers.

%prep
%setup -c -T

%files

%install
