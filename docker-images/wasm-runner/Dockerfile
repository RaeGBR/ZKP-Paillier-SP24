FROM polyubtc/cpp-lib-runner

RUN apt-get update
RUN apt-get install -y nodejs npm

# Solve SBT version mismatch problem
# RUN sudo rm -rf /etc/ssl/certs/java /etc/java-*
# RUN apt-get install -y openjdk-8-jdk
RUN update-ca-certificates -f

# EMSDK & WebAssembly Dependencies
RUN npm install -g mocha

RUN git clone https://github.com/juj/emsdk.git
RUN cd emsdk && git pull
RUN cd emsdk && ./emsdk install latest
RUN cd emsdk && ./emsdk activate latest
RUN cd emsdk && /bin/bash -c "source ./emsdk_env.sh"
