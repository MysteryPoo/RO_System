<template>
  <div>
    <q-linear-progress stripe rounded size="50px" :value="percent" class="q-mt-sm" />
    <p>About {{ remainingTime }} remaining.</p>
  </div>
</template>

<script setup lang="ts">
import { Ref, ref, onMounted, onBeforeUnmount } from 'vue';

interface ITimeProgress {
  asPercent: number,
  asTime: string,
}

const props = defineProps({
  startTime: {
    type: Date,
    required: false,
  },
  estimatedElapsedSeconds: {
    type: Number,
    required: false,
  },
});

const interval : Ref<number | NodeJS.Timeout | undefined> = ref(undefined);
const percent = ref(0);
const remainingTime = ref('unknown');

onMounted( () => {
  startProgress();
});

onBeforeUnmount( () => {
  endProgress();
});

const startProgress = () => {
  if (interval.value === undefined) {
    interval.value = setInterval( () => {
      const { asPercent, asTime } : ITimeProgress = calculateTimeProgress();
      percent.value = asPercent;
      remainingTime.value = asTime;
    }, 1000);
  }
};

const endProgress = () => {
  if (interval.value !== undefined) {
    clearInterval(interval.value);
    interval.value = undefined;
  }
}

const calculateTimeProgress = () => {
  if (undefined === props.startTime || undefined === props.estimatedElapsedSeconds) {
    return {
      asPercent: 0,
      asTime: '',
    };
  }
  const now = new Date();
  let estimatedCompletionTime = new Date(props.startTime);
  estimatedCompletionTime.setSeconds(props.startTime.getSeconds() + props.estimatedElapsedSeconds);
  const totalTime : number = estimatedCompletionTime.getTime() - props.startTime.getTime();
  const currentElapsedTime = now.getTime() - props.startTime.getTime();
  const remainingTime = estimatedCompletionTime.getTime() - now.getTime();
  const minutesRemaining = Math.round(remainingTime / 1000 / 60);
  const secondsRemaining = Math.round(remainingTime / 1000);
  return {
    asPercent: Math.min(currentElapsedTime / totalTime * 100, 100),
    asTime: minutesRemaining > 0 ? `${minutesRemaining} minute${minutesRemaining !== 1 ? 's' : ''}` : `${Math.max(0, secondsRemaining)} second${secondsRemaining !== 1 ? 's' : ''}`,
  };
};

</script>
