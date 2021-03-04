require 'rake/extensiontask'

task build: :compile

Rake::ExtensionTask.new('clang') do |ext|
  ext.lib_dir = 'lib/clang'
  ext.ext_dir = 'ext/clang'
  ext.source_pattern = '**/*.{c,h}'
  # ext.config_includes << 'ext/clang
end

task default: %i[clobber compile]
