import { createApp } from 'vue';
import App from './App.vue';
import PrimeVue from 'primevue/config';
import router from './router';

import 'primevue/resources/themes/lara-dark-indigo/theme.css';       //theme
import 'primevue/resources/primevue.min.css';                 //core css
import 'primeicons/primeicons.css';                           //icons
import 'primeflex/primeflex.css';

let app = createApp(App);
app.use(PrimeVue, {ripple: true});
app.use(router);

app.mount('#app');
