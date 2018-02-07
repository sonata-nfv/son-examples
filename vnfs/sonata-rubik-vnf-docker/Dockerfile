FROM ubuntu:xenial
LABEL maintainer="palemany@cttc.cat"

# define interface names (should be the same as in VNFD)
ENV IFIN input
ENV IFOUT output

RUN export DEBIAN_FRONTEND=noninteractive && apt-get update && apt-get install -y -q \
    net-tools \
    iproute \
    inetutils-ping \
    curl \
    python-setuptools \
    python-pip \
    libffi-dev

WORKDIR /RubikSolverServerApp

COPY . /RubikSolverServerApp

RUN pip install -r requirements.txt

# Make port 5000 available to the world outside this container
EXPOSE 5000

#CMD ["python", "RubikSolverServerApp.py"]
#ENTRYPOINT [ "python" ]
#CMD [ "RubikSolverServerApp.py" ]

# set entry point for emulator gatekeeper
ENV SON_EMU_CMD ./start.sh

# normal entry point
CMD /bin/bash

