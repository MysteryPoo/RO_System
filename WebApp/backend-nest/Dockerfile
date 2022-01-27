FROM node:16.13.2-alpine3.14 as build-stage
WORKDIR /app
COPY package*.json ./
RUN apk --no-cache add --virtual builds-deps build-base python3 && npm install && npm rebuild bcrypt --build-from-source
COPY ./ .

FROM node:16.13.2-alpine3.14 as production-stage
RUN mkdir /app
WORKDIR /app
COPY --from=build-stage /app /app
CMD ["npm", "start"]
