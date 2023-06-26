FROM node:latest AS deployment

RUN npm install -g serve

WORKDIR /api_server

ENV API_SERVER_IP=0.0.0.0
ENV API_SERVER_PORT=8080
EXPOSE 8080

COPY ./backend/build/api_server-0.1.0-Linux.deb .
RUN dpkg -i ./api_server-0.1.0-Linux.deb

WORKDIR /var/www/html

ENV CLIENT_IP=0.0.0.0
ENV CLIENT_PORT=3000
EXPOSE 3000

COPY ./frontend/build .
COPY ./docker/entrypoint.sh /
RUN chmod +x /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]