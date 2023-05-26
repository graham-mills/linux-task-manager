FROM ubuntu:22.04

WORKDIR /workspace

EXPOSE 8080/tcp
EXPOSE 3000/tcp

# Add non-root user and group
# ARG USER_ID
# ARG GROUP_ID
# RUN addgroup -gid $GROUP_ID user
# RUN adduser --disabled-password -gecos '' --uid $USER_ID --gid $GROUP_ID user

# Install dependencies
RUN apt update \ 
    && apt install -y python3 \
    python3-pip \
    cmake

# Install conan
RUN pip install conan
RUN conan profile detect

ENTRYPOINT [ "make" ]
