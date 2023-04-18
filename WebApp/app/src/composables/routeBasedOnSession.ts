import { onBeforeMount, ref } from 'vue';
import { useRouter } from 'vue-router';
import { supabase } from 'src/services/supabase.service';
import { useQuasar } from 'quasar';

export function useRouteBasedOnSession() {
  const router = useRouter();
  const quasar = useQuasar();
  const isValidSession = ref(false);

  onBeforeMount( async () => {
    const { data, error } = await supabase.auth.getSession();
    console.log('data:', data);
    console.log('error', error);
    isValidSession.value = data && data.session != null && !error;

    if (!isValidSession.value) {
      router.go(-window.history.length);
      router.push('/auth');
      quasar.notify({
        type: 'warning',
        message: 'Please sign in.',
        position: 'top',
    });
    }
  });

  return { isValidSession };
}
