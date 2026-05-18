---
layout: home
hero:
  name: Project Status
  text: ' '
  actions:
    - theme: brand
      text: 简体中文
      link: /zh/project-status/
    - theme: alt
      text: English
      link: /en/project-status/
---

<script setup>
import { onMounted } from 'vue'
import { useRouter, withBase } from 'vitepress'

onMounted(() => {
  const router = useRouter()
  const lang = navigator.language || navigator.userLanguage
  const target = lang.startsWith('zh') ? '/zh/project-status/' : '/en/project-status/'
  router.go(withBase(target))
})
</script>
