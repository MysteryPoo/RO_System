FROM node:lts-alpine3.13 as build-stage
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY ./ .

FROM node:lts-alpine3.13 as production-stage
RUN mkdir /app
WORKDIR /app
COPY --from=build-stage /app /app
CMD ["npm", "start"]
