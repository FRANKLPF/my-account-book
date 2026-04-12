# 家庭记账本 - AGENTS.md

## Architecture

Single-file SPA (`index.html`, ~3340 lines). All HTML, CSS, and JS in one file — no build step, no bundler, no Node dependencies.

- **Backend**: Supabase (Postgres + Auth), accessed via CDN `@supabase/supabase-js@2` (defer)
- **Charts**: Chart.js v4 — **lazy loaded** via `loadChartJs()` only when user navigates to chart pages
- **Fonts**: System fonts only (PingFang SC, Microsoft YaHei). No Google Fonts — they are blocked/slow in China.
- **Deploy**: Vercel + Cloudflare Pages (auto-deploy on `git push origin main`)
- **Production URL**: `https://accountbook.opensemi.xyz` (Cloudflare Pages custom domain)

### Supabase Client

Initialized as `db` (NOT `supabase`) because `window.supabase` conflicts with the CDN global. Initialization is deferred — `initSupabase()` called inside `initApp()` after DOMContentLoaded, waiting for the defer'd script to load:

```js
let db = null;
function initSupabase() {
    if (window.supabase && window.supabase.createClient) {
        db = window.supabase.createClient(SUPABASE_URL, SUPABASE_KEY);
        return true;
    }
    return false;
}
```

**Never rename `db` to `supabase`** — it will shadow the CDN global and break.

### Supabase Tables

- **`transactions`**: `id`, `date`, `type` (income/expense), `category`, `amount`, `description`, `person`, `user_id` (FK → auth.users), `created_at`
- **`snapshots`**: `id`, `date`, `cards` (JSONB), `card_balance`, `savings` (JSONB), `savings_balance`, `liabilities` (JSONB), `liabilities_balance`, `total_assets`, `balance_change`, `note`, `user_id`, `created_at`
- **`user_settings`**: `id`, `user_id` (UNIQUE FK → auth.users), `settings` (JSONB), `created_at`, `updated_at`

All tables have RLS: `USING (auth.uid() = user_id)`.

### Data Flow

- **All data operations go directly to Supabase** — no offline-first or batch sync. Every insert/delete/update is an `await` call to `db.from(...)`.
- `familyMembers`: synced via `user_settings` table. `localStorage` used as offline cache/fallback. `saveMembers()` calls both `localStorage.setItem()` and fire-and-forget `saveUserSettings()`.
- `transactions` / `snapshots`: loaded from Supabase on auth via `loadData()`, kept in memory.

### System Role: "资产统计"

Auto-generated reconciliation transactions use `person: '资产统计'` (not a real family member). When a snapshot is saved and a previous snapshot exists, the app calculates the unexplained asset difference and auto-inserts a transaction with category `资产统计正差` or `资产统计负差`.

**Exclusions**: "资产统计" is excluded from dashboard person cards, person pie chart, daily calendar heatmap, trend chart, and member management. It IS visible in transaction list and person filter dropdown.

### Loading Sequence

1. HTML renders immediately with loading animation (`#loadingScreen`)
2. Supabase SDK loads (defer'd script)
3. `initApp()` fires → `initSupabase()` → check auth session → `showApp()` or `showLogin()`
4. `showApp()` → `loadUserSettings()` → `loadData()` → render UI → `hideLoadingScreen()`
5. Chart.js only loads when user navigates to a chart page (`loadChartJs()`)

## Pages (SPA navigation via `navigateTo()`)

1. **Dashboard** (`page-dashboard`): Monthly summary, per-person expense cards, recent transactions
2. **Add Transaction** (`page-add`): Category grid, person selector, amount input. Shows "请先记录资产快照" notice if no snapshots exist — user must create first snapshot before recording transactions.
3. **Transactions** (`page-transactions`): Filterable list with type/person/category/date filters
4. **Assets** (`page-assets`): Snapshot list, multi-card + multi-savings + multi-liability input modal, net assets = positive - liabilities, asset trend chart, assessment
5. **Statistics** (`page-statistics`): Category doughnut, trend bar chart, person pie chart, daily calendar heatmap, reconciliation summary
6. **Settings** (`page-settings`): Member management, export/import, hidden "clear all data" button (opacity 0.3, hover to reveal)

## Gotchas

- **No `confirm()` or `alert()`**: App uses `showConfirm()` (returns Promise) and `showToast()`. Never add native dialogs.
- **No hardcoded person names**: All person references are dynamic via `familyMembers` array. Never hardcode "老公"/"老婆" in new code. Exception: the system role "资产统计" is a constant string.
- **Snapshot prefill**: `prefillSnapshotFromLast()` auto-fills card/savings/liability item names from the last snapshot (NOT amounts).
- **Delete member = delete transactions**: `removeMember()` calls `db.from('transactions').delete()` for that person.
- **`replaceAll` danger**: Never use `replaceAll('supabase', ...)` — it will corrupt the Supabase URL (`supabase.co`).
- **Git push from China**: Requires `git config --global http.version HTTP/1.1` (HTTP/2 gets reset). Connection is flaky — retry 2-3 times.
- **Domain blocking**: `.vercel.app` and `.pages.dev` are blocked in China. Must use custom domain `accountbook.opensemi.xyz`.
- **Chart.js is async**: All chart rendering functions are `async` and start with `await loadChartJs()`. Callers must also `await`.
- **Reconciliation categories not in selection grids**: `资产统计正差` and `资产统计负差` are in `CATEGORY_EMOJIS` but NOT in `EXPENSE_CATEGORIES`/`INCOME_CATEGORIES` — they are auto-generated only.

## Common Tasks

### Add a new page/section
1. Add `<section id="page-xxx" class="page">` in the HTML
2. Add nav item in `<nav class="bottom-nav">`
3. Wire up in `navigateTo()` function
4. Call relevant update functions

### Add a new data field to transactions/snapshots
1. ALTER TABLE in Supabase SQL Editor
2. Update `loadData()` mapping
3. Update insert in `handleTransactionSubmit()` or `handleSnapshotSubmit()`
4. Update display templates (dashboard, list, etc.)

### Modify categories
Edit `EXPENSE_CATEGORIES`, `INCOME_CATEGORIES`, and `CATEGORY_EMOJIS` at ~line 1297.

## Deployment

```bash
# Push triggers auto-deploy to both Vercel and Cloudflare Pages
git add . && git commit -m "feat: description" && git push origin main

# If push fails (connection reset), retry or set:
git config --global http.version HTTP/1.1
git config --global http.postBuffer 524288000
```

## Pending Infrastructure

All SQL tables have been created. No pending infrastructure tasks.
