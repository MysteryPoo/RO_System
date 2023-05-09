<template>
  <div>
    <div class="text-h6" v-if="float !== undefined">Float is <span :style="float ? 'color: red' : 'color: green'">{{ float ? "Full" : "Not Full" }}</span></div>
  </div>
</template>

<script setup lang="ts">
import { RealtimeChannel } from '@supabase/realtime-js';
import { useDeviceStore } from 'src/stores/device-store';
import { supabase, StateRequestRowTypes, StateRequestSelect, StateRequestInsert } from 'src/services/supabase.service';
import { ref, Ref, onBeforeUnmount, onMounted, watch } from 'vue';
</script>
