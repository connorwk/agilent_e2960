FROM ubuntu:latest

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

RUN apt-get update \
    && apt-get install -y vsftpd \
    && sed -i 's/listen=NO/listen=YES/' /etc/vsftpd.conf \
    && sed -i 's/listen_ipv6=YES/listen_ipv6=NO\nlisten_address=10.0.0.1\nbackground=NO/' /etc/vsftpd.conf \
    && sed -i 's/anonymous_enable=NO/anonymous_enable=YES/g' /etc/vsftpd.conf \
    && sed -i 's/local_enable=YES/local_enable=NO/g' /etc/vsftpd.conf \
    && mkdir -p /var/run/vsftpd/empty


ENTRYPOINT ["/usr/sbin/vsftpd","/etc/vsftpd.conf"]
