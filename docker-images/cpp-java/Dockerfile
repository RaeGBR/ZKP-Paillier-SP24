FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y g++
RUN apt-get install -y make
RUN apt-get install -y openjdk-8-jdk
RUN apt-get install -y rsync
RUN apt-get install -y wget unzip
RUN wget https://services.gradle.org/distributions/gradle-4.7-bin.zip
RUN mkdir /opt/gradle
RUN unzip -d /opt/gradle gradle-4.7-bin.zip
ENV PATH="${PATH}:/opt/gradle/gradle-4.7/bin"
RUN apt-get install -y lcov
