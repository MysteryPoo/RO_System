FROM mcr.microsoft.com/dotnet/sdk:7.0-alpine AS build
WORKDIR /app

# copy csproj file and restore
COPY ./WebApp/MQTT_Discovery_Service/*.csproj ./MQTT_Discovery_Service/
RUN dotnet restore ./MQTT_Discovery_Service

# Copy everything else and build
COPY ./WebApp/MQTT_Discovery_Service ./MQTT_Discovery_Service
RUN dotnet publish ./MQTT_Discovery_Service -c Release -o out

# Build runtime image
FROM mcr.microsoft.com/dotnet/aspnet:7.0-alpine
WORKDIR /app
COPY --from=build /app/out .
ENTRYPOINT ["dotnet", "MQTT_Discovery_Service.dll"]
