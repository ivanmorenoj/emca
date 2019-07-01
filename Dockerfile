FROM ivan28823/emca:mariadb

RUN apt update && apt -y upgrade

RUN mkdir -p /etc/emca/
RUN mkdir -p /var/log/emca/

COPY build/bin/emca /bin/emca


CMD /usr/sbin/service mysql start && /bin/emca