---
layout: home
title: Status
text: ' '
hero:
  actions:
    - theme: brand
      text: 简体中文
      link: /zh/status/
    - theme: alt
      text: English
      link: /en/status/
---

<script setup>
import { onMounted } from 'vue'
import { useRouter, withBase } from 'vitepress'

const router = useRouter()

onMounted(() => {
  const lang = navigator.language || navigator.userLanguage || ''
  const target = lang.startsWith('zh') ? '/zh/status/' : '/en/status/'
  router.go(withBase(target))
})
</script>
