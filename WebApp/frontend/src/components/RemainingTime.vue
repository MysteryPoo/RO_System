<template>
  <div>
    <ProgressBar :value="percent" :showValue="false" />
    <p>About {{ remainingTime }} remaining.</p>
  </div>
</template>

<script setup>
import { ref, defineProps, onMounted, onBeforeUnmount } from 'vue';
import ProgressBar from 'primevue/progressbar';

const props = defineProps({
  startTime: Date,
  estimatedElapsedSeconds: Number,
});

const interval = ref(null);
const percent = ref(0);
const remainingTime = ref('unknown');

onMounted( () => {
  startProgress();
});

onBeforeUnmount( () => {
  endProgress();
});

const startProgress = () => {
  if (interval.value === null) {
    interval.value = setInterval( () => {
      const { asPercent, asTime } = calculateTimeProgress();
      percent.value = asPercent;
      remainingTime.value = asTime;
    }, 1000);
  }
};

const endProgress = () => {
  if (interval.value !== null) {
    clearInterval(interval.value);
    interval.value = null;
  }
}

const calculateTimeProgress = () => {
  const now = new Date();
  let estimatedCompletionTime = new Date(props.startTime);
  estimatedCompletionTime.setSeconds(props.startTime.getSeconds() + props.estimatedElapsedSeconds);
  const totalTime = estimatedCompletionTime - props.startTime;
  const currentElapsedTime = now - props.startTime;
  const remainingTime = estimatedCompletionTime - now;
  const minutesRemaining = Math.round(remainingTime / 1000 / 60);
  const secondsRemaining = Math.round(remainingTime / 1000);
  return {
    asPercent: Math.min(currentElapsedTime / totalTime * 100, 100),
    asTime: minutesRemaining > 0 ? `${minutesRemaining} minute${minutesRemaining !== 1 ? 's' : ''}` : `${Math.max(0, secondsRemaining)} second${secondsRemaining !== 1 ? 's' : ''}`,
  };
};
</script>
