module.exports = {
  ignorePatterns: ['/docs/'],
  env: {
    es2017: true,
    es2020: true,
    es6: true,
    node: true,
  },
  extends: ['plugin:prettier/recommended', 'plugin:markdown/recommended'],
  parserOptions: {
    ecmaVersion: 11,
  },
  plugins: ['prettier', 'markdown'],
  rules: {
    'prettier/prettier': 'error',
    'spaced-comment': ['error', 'always'],
  },
};
