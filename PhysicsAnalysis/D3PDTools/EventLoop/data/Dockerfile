#
# Image providing the project on top of an AnalysisBase image.
#

# docker build -f /path/to/dockerfile /path/to/context --build-arg RELEASE=atlas/analysisbase:21.2.12

ARG RELEASE=atlas/analysisbase
#ARG RELEASE=atlas/analysisbase:21.2.4

# Set up the base image:
FROM ${RELEASE} as builder

# Copy the project's sources into the image:
COPY . source

# Build the project inside a build/ directory:
RUN source /home/atlas/release_setup.sh && \
    mkdir build && \
    cd build/ && \
    cmake -DATLAS_PACKAGE_FILTER_FILE=../source/el_docker_data/package_filters.txt ../source/ && \
    cmake --build . && \
    cpack && \
    sudo rpm -i /home/atlas/build/*.rpm

FROM ${RELEASE}

ARG PROJECT_NAME

ENV PROJECT_NAME ${PROJECT_NAME}

COPY --from=builder /usr/${PROJECT_NAME} /usr/${PROJECT_NAME}
COPY --from=builder /home/atlas/source/el_docker_data/setup.sh /home/atlas/setup.sh

# Set up the environment setup script:
RUN mv release_setup.sh analysis_release_setup.sh

# Start the image with BASH by default:
CMD /bin/bash
