<template>
  <div class="text-h6">{{ $props.pressure }} {{ $props.pressure == 1 ? units : `${units}s` }}</div>
</template>

<script setup lang="ts">

defineProps<{
  pressure: number | undefined,
  units: string,
}>();

</script>
