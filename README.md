# 🏠 家庭记账本

一个简洁的家庭记账 Web 应用，支持多成员记账、资产快照、统计分析，数据云端同步，多设备共享。

**在线使用**：[https://accountbook.opensemi.xyz](https://accountbook.opensemi.xyz)

## ✨ 功能特性

- 📝 **收支记录** — 支出/收入分类记录，支持自定义家庭成员
- 💰 **资产快照** — 多银行卡、多储蓄账户、多负债，净资产自动计算
- 📊 **统计分析** — 分类饼图、月度趋势、人员对比、每日日历热力图
- 🔄 **资产对账** — 自动计算资产差额，生成正差/负差记录
- 👥 **多成员管理** — 自定义添加家庭成员，数据跨设备同步
- 🔐 **账号系统** — 邮箱注册登录，数据隔离，隐私安全
- ☁️ **云端同步** — Supabase 实时数据库，手机电脑实时同步
- 📱 **移动适配** — 响应式设计，手机端完美体验

## 🛠 技术栈

| 组件 | 技术 |
|------|------|
| 前端 | 原生 HTML/CSS/JS（单文件 SPA） |
| 后端 | Supabase（Postgres + Auth） |
| 图表 | Chart.js v4（按需加载） |
| 字体 | 系统字体（PingFang SC / 微软雅黑） |
| 部署 | Cloudflare Pages + Vercel |
| 域名 | accountbook.opensemi.xyz |

## 🚀 部署

项目为零依赖单文件应用，推送到 GitHub 后自动部署：

```bash
git add . && git commit -m "feat: description" && git push origin main
```

### 从零搭建

1. Fork 或 clone 本仓库
2. 在 [Supabase](https://supabase.com) 创建项目
3. 在 SQL Editor 中执行建表语句（见 `index.html` 顶部注释）
4. 将 Supabase URL 和 Key 替换到 `index.html` 中的 `SUPABASE_URL` 和 `SUPABASE_KEY`
5. 推送到 GitHub，连接 Cloudflare Pages 或 Vercel 自动部署
6. 绑定自定义域名（国内必须，`.pages.dev` 和 `.vercel.app` 被墙）

## 📄 文件结构

```
├── index.html      # 主应用（单文件，包含全部 HTML/CSS/JS）
├── vercel.json     # Vercel 部署配置
├── wrangler.jsonc  # Cloudflare Pages 配置
├── AGENTS.md       # 开发指南（给 AI agent 用）
└── USER_GUIDE.md   # 用户使用手册
```

## 📖 使用说明

详见 [用户使用手册](./USER_GUIDE.md)

## 📜 License

MIT
