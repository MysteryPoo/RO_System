<template>
    <div>

    </div>
</template>

<script setup>
import { onMounted } from 'vue';
import { useRouter } from 'vue-router';
import { useStore } from 'vuex';

const router = useRouter();
const store = useStore();

onMounted( () => {
    window.localStorage.removeItem('token');
    store.commit('set_token', undefined);
    router.replace('/login');
});
</script>
