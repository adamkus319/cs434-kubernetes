# Dockerfile
FROM debian:latest
RUN apt-get update && apt-get install -y libc6 libgcc1
COPY ./messagebus/mbtest /usr/bin/mbtest
RUN chmod +x /usr/bin/mbtest
CMD ["mbtest"]
