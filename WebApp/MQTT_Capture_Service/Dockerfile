FROM mcr.microsoft.com/dotnet/sdk:7.0.202-bullseye-slim-amd64 AS build

ARG TARGETARCH
ARG TARGETOS

RUN arch=$TARGETARCH \
  && if [ "$arch" = "amd64" ]; then arch="x64"; fi \
  && echo $TARGETOS-$arch > /tmp/rid

WORKDIR /app
# copy csproj file and restore
COPY ./MQTT_Capture_Service/*.csproj ./MQTT_Capture_Service/
RUN dotnet restore ./MQTT_Capture_Service -r $(cat /tmp/rid)

# Copy everything else and build
COPY ./MQTT_Capture_Service ./MQTT_Capture_Service
RUN dotnet publish ./MQTT_Capture_Service -c Release -o out -r $(cat /tmp/rid) --self-contained false --no-restore

# Build runtime image
FROM mcr.microsoft.com/dotnet/runtime:7.0.4-alpine3.17
WORKDIR /app
COPY --from=build /app/out .
ENTRYPOINT ["dotnet", "MQTT_Capture_Service.dll"]
