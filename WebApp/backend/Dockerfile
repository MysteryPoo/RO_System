FROM node:16.13.2 as build-stage
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY ./ .
RUN npm run build

FROM node:16.13.2-alpine3.14 as production-stage
RUN mkdir /app
WORKDIR /app
COPY --from=build-stage /app /app
CMD ["npm", "run", "start:prod"]
