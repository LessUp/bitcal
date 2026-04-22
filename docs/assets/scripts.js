/**
 * BitCal Documentation - Shared Scripts
 */

(function() {
    'use strict';

    // ============================================
    // Theme Management
    // ============================================
    const Theme = {
        init() {
            const saved = localStorage.getItem('bitcal-theme') || 'dark';
            this.set(saved);
            
            // Setup toggle buttons
            document.querySelectorAll('[data-theme-toggle]').forEach(btn => {
                btn.addEventListener('click', () => this.toggle());
            });
        },

        set(theme) {
            document.documentElement.setAttribute('data-theme', theme);
            localStorage.setItem('bitcal-theme', theme);
            
            // Update all toggle icons
            document.querySelectorAll('[data-theme-icon]').forEach(icon => {
                icon.className = theme === 'dark' ? 'fas fa-sun' : 'fas fa-moon';
            });
        },

        toggle() {
            const current = document.documentElement.getAttribute('data-theme') || 'dark';
            this.set(current === 'dark' ? 'light' : 'dark');
        }
    };

    // ============================================
    // Language Management
    // ============================================
    const I18n = {
        currentLang: 'en',

        init() {
            // Detect language from URL or localStorage
            const pathLang = window.location.pathname.includes('/zh/') ? 'zh' : 'en';
            this.currentLang = pathLang;
            
            // Update all lang toggle buttons
            document.querySelectorAll('[data-lang-toggle]').forEach(btn => {
                btn.addEventListener('click', () => this.toggle());
            });
        },

        toggle() {
            const current = window.location.pathname.includes('/zh/') ? 'zh' : 'en';
            const target = current === 'en' ? 'zh' : 'en';
            
            // Replace language in URL
            let newPath = window.location.pathname;
            if (current === 'en') {
                newPath = newPath.replace('/docs/en/', '/docs/zh/');
                if (!window.location.pathname.includes('/docs/')) {
                    newPath = window.location.pathname.replace('/bitcal/', '/bitcal/zh/');
                }
            } else {
                newPath = newPath.replace('/docs/zh/', '/docs/en/');
                newPath = newPath.replace('/zh/', '/');
            }
            
            // Fallback to home if path doesn't exist
            window.location.href = newPath || (target === 'zh' ? './zh/' : './');
        },

        getLangFromPath() {
            return window.location.pathname.includes('/zh/') ? 'zh' : 'en';
        }
    };

    // ============================================
    // Table of Contents Generation
    // ============================================
    const TOC = {
        init() {
            const tocNav = document.querySelector('.toc-nav');
            if (!tocNav) return;

            const content = document.querySelector('.content');
            if (!content) return;

            const headings = content.querySelectorAll('h2, h3, h4');
            if (headings.length === 0) {
                document.querySelector('.toc-sidebar')?.classList.add('hidden');
                document.body.classList.remove('with-toc');
                return;
            }

            // Generate TOC
            let html = '';
            headings.forEach((heading, index) => {
                const id = heading.id || `heading-${index}`;
                heading.id = id;
                
                const level = parseInt(heading.tagName[1]);
                const text = heading.textContent;
                
                html += `<li class="toc-h${level}"><a href="#${id}" data-toc-link>${text}</a></li>`;
            });

            tocNav.innerHTML = html;

            // Add click handlers
            tocNav.querySelectorAll('a').forEach(link => {
                link.addEventListener('click', (e) => {
                    e.preventDefault();
                    const target = document.querySelector(link.getAttribute('href'));
                    if (target) {
                        target.scrollIntoView({ behavior: 'smooth', block: 'start' });
                        history.pushState(null, null, link.getAttribute('href'));
                    }
                });
            });

            // Setup intersection observer
            this.setupObserver(headings);
        },

        setupObserver(headings) {
            const tocLinks = document.querySelectorAll('[data-toc-link]');
            
            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting) {
                        const id = entry.target.id;
                        tocLinks.forEach(link => {
                            link.classList.remove('active');
                            if (link.getAttribute('href') === `#${id}`) {
                                link.classList.add('active');
                            }
                        });
                    }
                });
            }, {
                rootMargin: '-20% 0px -60% 0px'
            });

            headings.forEach(h => observer.observe(h));
        }
    };

    // ============================================
    // Code Copy Buttons
    // ============================================
    const CodeBlocks = {
        init() {
            document.querySelectorAll('pre').forEach(pre => {
                // Skip if already wrapped
                if (pre.parentElement.classList.contains('code-block')) return;

                const wrapper = document.createElement('div');
                wrapper.className = 'code-block';
                pre.parentNode.insertBefore(wrapper, pre);
                wrapper.appendChild(pre);

                const btn = document.createElement('button');
                btn.className = 'btn btn-sm btn-secondary copy-btn';
                btn.innerHTML = '<i class="fas fa-copy"></i>';
                btn.title = 'Copy to clipboard';
                
                btn.addEventListener('click', async () => {
                    const code = pre.querySelector('code') || pre;
                    try {
                        await navigator.clipboard.writeText(code.textContent);
                        btn.classList.add('copied');
                        btn.innerHTML = '<i class="fas fa-check"></i>';
                        setTimeout(() => {
                            btn.classList.remove('copied');
                            btn.innerHTML = '<i class="fas fa-copy"></i>';
                        }, 2000);
                    } catch (err) {
                        console.error('Failed to copy:', err);
                    }
                });

                wrapper.appendChild(btn);
            });
        }
    };

    // ============================================
    // Mobile Menu
    // ============================================
    const MobileMenu = {
        init() {
            const toggle = document.querySelector('[data-mobile-toggle]');
            const sidebar = document.querySelector('.sidebar');
            
            if (!toggle || !sidebar) return;

            toggle.addEventListener('click', () => {
                sidebar.classList.toggle('open');
            });

            // Close on click outside
            document.addEventListener('click', (e) => {
                if (!sidebar.contains(e.target) && !toggle.contains(e.target)) {
                    sidebar.classList.remove('open');
                }
            });

            // Close on link click
            sidebar.querySelectorAll('a').forEach(link => {
                link.addEventListener('click', () => {
                    sidebar.classList.remove('open');
                });
            });
        }
    };

    // ============================================
    // Search
    // ============================================
    const Search = {
        index: [],

        async init() {
            const searchInput = document.querySelector('.search-input');
            const searchResults = document.querySelector('.search-results');
            
            if (!searchInput) return;

            // Load search index
            try {
                const response = await fetch('./assets/search-index.json');
                this.index = await response.json();
            } catch (e) {
                console.log('Search index not available');
                return;
            }

            // Setup search
            let debounceTimer;
            searchInput.addEventListener('input', (e) => {
                clearTimeout(debounceTimer);
                debounceTimer = setTimeout(() => {
                    this.search(e.target.value, searchResults);
                }, 150);
            });

            // Close on click outside
            document.addEventListener('click', (e) => {
                if (!searchInput.contains(e.target) && !searchResults?.contains(e.target)) {
                    searchResults?.classList.remove('active');
                }
            });

            // Keyboard shortcut
            document.addEventListener('keydown', (e) => {
                if ((e.metaKey || e.ctrlKey) && e.key === 'k') {
                    e.preventDefault();
                    searchInput.focus();
                }
            });
        },

        search(query, resultsContainer) {
            if (!query || query.length < 2) {
                resultsContainer?.classList.remove('active');
                return;
            }

            const terms = query.toLowerCase().split(' ');
            const results = this.index
                .map(item => {
                    let score = 0;
                    const text = (item.title + ' ' + item.content).toLowerCase();
                    
                    terms.forEach(term => {
                        if (item.title.toLowerCase().includes(term)) score += 10;
                        if (text.includes(term)) score += 1;
                    });
                    
                    return { ...item, score };
                })
                .filter(item => item.score > 0)
                .sort((a, b) => b.score - a.score)
                .slice(0, 10);

            if (results.length === 0) {
                resultsContainer.innerHTML = '<div class="search-result-item"><span class="text-muted">No results found</span></div>';
            } else {
                resultsContainer.innerHTML = results.map(r => `
                    <div class="search-result-item" onclick="location.href='${r.url}'">
                        <div class="search-result-title">${this.highlight(r.title)}</div>
                        <div class="search-result-path">${r.path}</div>
                    </div>
                `).join('');
            }

            resultsContainer?.classList.add('active');
        },

        highlight(text) {
            return text; // Simplified - could add highlight spans
        }
    };

    // ============================================
    // Initialize
    // ============================================
    document.addEventListener('DOMContentLoaded', () => {
        Theme.init();
        I18n.init();
        TOC.init();
        CodeBlocks.init();
        MobileMenu.init();
        Search.init();
    });

    // Expose to global for inline handlers if needed
    window.BitCalDoc = { Theme, I18n };
})();
