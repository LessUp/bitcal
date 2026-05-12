---
layout: home
hero:
  name: BitCal
  text: ' '
  actions:
    - theme: brand
      text: 简体中文
      link: /zh/
    - theme: alt
      text: English
      link: /en/
---

<script setup>
import { onMounted } from 'vue'
import { useRouter, useData, withBase } from 'vitepress'

onMounted(() => {
  const router = useRouter()
  const lang = navigator.language || navigator.userLanguage
  const target = lang.startsWith('zh') ? '/zh/' : '/en/'
  router.go(withBase(target))
})
</script>
