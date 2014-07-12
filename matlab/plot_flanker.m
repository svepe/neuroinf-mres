n_ch = size(eeg, 2) - 2;

for i = 4:1:n_ch
	plot(eeg(:, 1), eeg(:, i))
	hold all;
end

for i = 1:1:size(event, 1)
	line([event(i, 1) event(i, 1)], [0  2000 * (event(i, 2)+1)]);
end
