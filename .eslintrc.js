module.exports = {
  env: {
    es2022: true,
    node: true,
  },
  extends: ['plugin:prettier/recommended', 'plugin:markdown/recommended'],
  plugins: ['prettier', 'markdown'],
  rules: {
    'prettier/prettier': 'error',
    'spaced-comment': ['error', 'always'],
  },
  ignorePatterns: ['/docs/'],
};
