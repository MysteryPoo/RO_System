###################
# BUILD FOR LOCAL DEVELOPMENT
###################

FROM node:18-bullseye As development

WORKDIR /usr/src/app

COPY --chown=node:node package*.json ./

RUN npm ci

USER node

###################
# BUILD FOR PRODUCTION
###################

FROM node:18-bullseye As build

WORKDIR /usr/src/app

COPY --chown=node:node package*.json ./
COPY --chown=node:node --from=development /usr/src/app/node_modules ./node_modules
COPY --chown=node:node . .

RUN npm run build

ENV NODE_ENV production

RUN npm ci --only=production && npm cache clean --force

USER node

###################
# PRODUCTION
###################

FROM nginx:1.23.4-alpine3.17 As production

WORKDIR /usr/src/app

COPY --chown=node:node --from=build /usr/src/app/package.json .
COPY --chown=node:node --from=build /usr/src/app/node_modules ./node_modules
COPY --chown=node:node --from=build /usr/src/app/dist ./dist
COPY nginx.conf /etc/nginx/nginx.conf
