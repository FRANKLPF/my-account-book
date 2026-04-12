# 家庭记账本 - AGENTS.md

## Architecture

Single-file SPA (`index.html`, ~2900 lines). All HTML, CSS, and JS in one file — no build step, no bundler, no Node dependencies.

- **Backend**: Supabase (Postgres + Auth), accessed via CDN-loaded `@supabase/supabase-js@2`
- **Charts**: Chart.js v4 via CDN
- **Fonts**: Google Fonts (Noto Sans SC, Oswald)
- **Deploy**: Vercel + Cloudflare Pages (auto-deploy on `git push origin main`)
- **Custom domain**: `opensemi.xyz` (binds to Cloudflare Pages)

### Supabase Client

The client is initialized as `db` (NOT `supabase`) because `window.supabase` conflicts with the CDN global:

```js
// Line ~1274-1276
const SUPABASE_URL = 'https://aglqeevuklpofovrwyjw.supabase.co';
const SUPABASE_KEY = 'eyJhbGciOiJIUzI1NiIs...';
let db = window.supabase.createClient(SUPABASE_URL, SUPABASE_KEY);
```

**Never rename `db` to `supabase`** — it will shadow the CDN global and break.

### Supabase Tables

- **`transactions`**: `id`, `date`, `type` (income/expense), `category`, `amount`, `description`, `person`, `user_id` (FK → auth.users), `created_at`
- **`snapshots`**: `id`, `date`, `cards` (JSONB array `{name, balance}`), `card_balance`, `savings` (JSONB), `savings_balance`, `total_assets`, `balance_change`, `note`, `user_id`, `created_at`
- **`user_settings`**: `id`, `user_id` (UNIQUE FK → auth.users), `settings` (JSONB), `created_at`, `updated_at`

All tables have RLS: `USING (auth.uid() = user_id)`.

### Local State

- `familyMembers`: synced via Supabase `user_settings` table (JSONB). `localStorage` used as offline cache. Default `["老公","老婆"]`.
- `transactions` / `snapshots`: loaded from Supabase on auth, kept in memory.

### Key JS State Variables (~line 1282)

```js
let transactions = [];
let snapshots = [];
let currentTransactionType = 'expense';
let currentCategory = null;
let currentPerson = '';    // defaults to familyMembers[0]
let charts = {};
let familyMembers = JSON.parse(localStorage.getItem('familyMembers') || '["老公","老婆"]');
```

## Pages (SPA navigation via `navigateTo()`)

1. **Dashboard** (`page-dashboard`): Monthly summary, per-person expense cards, recent transactions
2. **Add Transaction** (`page-add`): Category grid, person selector, amount input
3. **Transactions** (`page-transactions`): Filterable list with type/person/category/date filters
4. **Assets** (`page-assets`): Snapshot list, multi-card + multi-savings input modal, asset trend chart, assessment
5. **Statistics** (`page-statistics`): Category doughnut, trend bar chart, person pie chart, daily calendar heatmap
6. **Settings** (`page-settings`): Member management, export/import, hidden "clear all data" button

## Gotchas

- **No `confirm()` or `alert()`**: App uses a custom modal (`showConfirm()` returns Promise). Never add native dialogs.
- **No hardcoded person names**: All person references are dynamic via `familyMembers` array. Never hardcode "老公"/"老婆" in new code.
- **Snapshot prefill**: `prefillSnapshotFromLast()` auto-fills card/savings item names from the last snapshot (not amounts).
- **Delete member = delete transactions**: `removeMember()` calls `db.from('transactions').delete()` for that person before updating local state.
- **`replaceAll` danger**: Never use `replaceAll('supabase', ...)` — it will corrupt the Supabase URL (`supabase.co`).
- **Git push from China**: Requires `git config --global http.version HTTP/1.1` (HTTP/2 gets reset).
- **Domain blocking**: `.vercel.app` and `.pages.dev` are blocked in China. Must use custom domain `opensemi.xyz`.

## Common Tasks

### Add a new page/section
1. Add `<section id="page-xxx" class="page">` in the HTML
2. Add nav item in `<nav class="bottom-nav">`
3. Wire up in `navigateTo()` function
4. Call relevant update functions

### Add a new data field to transactions/snapshots
1. ALTER TABLE in Supabase SQL Editor
2. Update `loadData()` mapping (~line 1390)
3. Update insert in `handleTransactionSubmit()` or `handleSnapshotSubmit()`
4. Update display templates (dashboard, list, etc.)

### Modify categories
Edit `EXPENSE_CATEGORIES`, `INCOME_CATEGORIES`, and `CATEGORY_EMOJIS` at ~line 1251.

## Deployment

```bash
# Push triggers auto-deploy to both Vercel and Cloudflare Pages
git add . && git commit -m "feat: description" && git push origin main

# If push fails (connection reset), this fixes it:
git config --global http.version HTTP/1.1
```

## Pending Infrastructure (user action required)

- [ ] Supabase SQL: `DROP TABLE IF EXISTS snapshots CASCADE` + rebuild with current schema (cards JSONB, savings JSONB, balance_change, total_assets)
- [ ] Supabase SQL: Create `user_settings` table (see SQL comment in code)
- [ ] Cloudflare Pages: bind custom domain `opensemi.xyz`
- [ ] `ALTER TABLE transactions DROP CONSTRAINT IF EXISTS transactions_person_check`
