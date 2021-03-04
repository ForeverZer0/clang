require_relative 'clang/version'
require_relative 'clang/clang'

module Clang

  # PATH = '/usr/include/GLFW/glfw3.h'
  PATH = '/usr/include/clang-c/Index.h'


  args = %w[--language=c --std=c17 --include-directory=/usr/include/linux --include-directory=/usr/include]
  Index.create(true, true) do |index|

    unit = TranslationUnit.parse(index, PATH, args)


    p Cursor.default_policy


  end

end
